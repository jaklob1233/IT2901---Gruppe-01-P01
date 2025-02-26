/*
 lyra_dec.cc

 Norman JANSEN
 Fraunhofer FKIE
 */
#include "mediastreamer2/mscodecutils.h"
#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/msticker.h"
#include "ortp/rtp.h"

#include <cstdint>
#include <filesystem>

extern "C" {
#include "params.h"
}

// from lyra
#include "lyra/lyra_decoder.h"
#include <memory>
#include <optional>

#include "absl/status/status.h"
#include "absl/types/span.h"
#include "include/ghc/filesystem.hpp"

/*
 * helper
 */
enum lyra_rate {
    LYRA_RATE_3200, /**< 3200 bit/s */
    LYRA_RATE_6000, /**< 6000 bit/s */
    LYRA_RATE_9200  /**< 9200 bit/s */
};

static int bitrateToConfigIndex(int bitrate) {
    switch (bitrate) {
        case 3200:
            return LYRA_RATE_3200;
        case 6000:
            return LYRA_RATE_6000;
        case 9200:
            return LYRA_RATE_9200;
        default:
            ms_fatal("bad LYRA bitrate %d", bitrate);
    }
}

/*filter common method*/
struct lyra_dec_struct {
    int sampleRate;
    int bitRate;
    int nchannels;
    MSConcealerContext *concealer;
    unsigned short int sequence_number;
    FILE* file;     /* sink (optional)           */
};

std::unique_ptr<chromemedia::codec::LyraDecoder> lyraDecoder;
bool lyraDecoderInitialized = false;

static void filter_init(MSFilter *f) {
    ms_message("Lyra decoder - lyra_enc->filter_init called");

    f->data = ms_new0(struct lyra_dec_struct, 1);
    auto *obj = (struct lyra_dec_struct *) f->data;

    obj->nchannels = 1;

    if(strcmp(f->desc->enc_fmt, "LYRA3200") == 0) {
        obj->bitRate = 3200;
    }
    else if(strcmp(f->desc->enc_fmt, "LYRA6000") == 0) {
        obj->bitRate = 6000;
    }
    else if(strcmp(f->desc->enc_fmt, "LYRA9200") == 0) {
        obj->bitRate = 9200;
    }
    else {
        ms_fatal("Lyra encoder - Unsupported MIME type [%s] for lyra", f->desc->enc_fmt);
    }
    ms_message("Lyra encoder - bitrate set to %d", obj->bitRate);

    params_lock();
    params_t *p = params(bitrateToConfigIndex(obj->bitRate));
    obj->file = p->fout ? fopen(p->fout, "w") : NULL;
    params_unlock();
}

static void filter_preprocess(MSFilter *f) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    const ghc::filesystem::path model_path = "/opt/linphone-sdk/lib/lyra_model_coeffs";
    std::filesystem::path cwd = std::filesystem::current_path();
    ms_message("Lyra decoder - current path: %s", cwd.c_str());

    lyraDecoder = chromemedia::codec::LyraDecoder::Create(/*sample_rate_hz=*/obj->sampleRate,
            /*num_channels=*/1,
            /*model_path=*/model_path);

    if (lyraDecoder == nullptr) {
        ms_fatal("Lyra decoder - could not create lyra decoder.");
    } else {
        lyraDecoderInitialized = true;
        ms_message("Lyra decoder - lyra decoder created");
    }
    obj->concealer = ms_concealer_context_new(UINT32_MAX);
}

/**
 put im to NULL for PLC
 */
static void decode(MSFilter *f, mblk_t *im, int num_samples_per_packet, int packet_size) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    mblk_t *om;
    std::optional<std::vector<int16_t>> decoded;

    om = allocb(num_samples_per_packet * 2, 0);

    if (im != nullptr) {
        mblk_meta_copy(im, om);

        // copy input buffer (im) from mediastreamer2 to input ConstSpan for lyra
        const absl::Span<const uint8_t> encoded_packet =
                absl::MakeConstSpan(im->b_rptr, packet_size);
        im->b_rptr += packet_size;

        if (!lyraDecoder->SetEncodedPacket(encoded_packet)) {
            ms_fatal("Lyra decoder - Unable to set encoded packet.");
        }
    } else {
        ms_message("Lyra decoder - Decoding packet in PLC mode.");
    }

    // decode with lyra
    decoded = lyraDecoder->DecodeSamples(num_samples_per_packet);

    if (!decoded.has_value()) {
        ms_error("Lyra decoder - Unable to decode features.");
        freeb(om);
    } else {
        // write encoded data vector from lyra to output buffer (om) for mediastreamer
        auto *buff_16 = (int16_t *) om->b_wptr;
        std::copy(decoded->begin(), decoded->end(), buff_16);

        if (obj->file)     /* if file is available, record frame */
            fwrite(buff_16, 1, num_samples_per_packet * 2, obj->file);

        om->b_wptr += num_samples_per_packet * 2;
        mblk_set_plc_flag(om, (im != nullptr) ? 0 : 1);
        ms_queue_put(f->outputs[0], om);
//        ms_message("Lyra decoder - decoded output of length %zu written to output buffer", decoded->size());
    }

    if (im) {
        obj->sequence_number = mblk_get_cseq(im);
    } else {
        obj->sequence_number++;
    }

    ms_concealer_inc_sample_time(obj->concealer, f->ticker->time, 20, im != nullptr);
}

static void filter_process(MSFilter *f) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    mblk_t *im;
    int nBytes;

    const int packet_size = std::ceil(static_cast<float>(obj->bitRate) / (lyraDecoder->frame_rate() * CHAR_BIT));
    const int num_samples_per_packet = lyraDecoder->sample_rate_hz() / lyraDecoder->frame_rate();
//    ms_message("Lyra decoder - calculated packetSize %d num_samples_per_packet %d", packet_size,
//               num_samples_per_packet);

    if (lyraDecoder->sample_rate_hz() % lyraDecoder->frame_rate() != 0) {
        ms_error("samplerate (%d) is not multiple of framerate (%d)", lyraDecoder->sample_rate_hz(),
                 lyraDecoder->frame_rate());
    }

    while ((im = ms_queue_get(f->inputs[0]))) {
        nBytes = im->b_wptr - im->b_rptr;
        ms_message("Lyra decoder - there are %d bytes in buffer", nBytes);

        while (nBytes >= packet_size) {
            decode(f, im, num_samples_per_packet, packet_size);

            nBytes = im->b_wptr - im->b_rptr;
            ms_message("Lyra decoder - there are %d bytes left in buffer", nBytes);
            /* Until last 20 ms frame of packet has been decoded */
        }

        freemsg(im);
    }

    while (ms_concealer_context_is_concealement_required(obj->concealer, f->ticker->time)) {
        decode(f, nullptr, num_samples_per_packet, packet_size);
    }
}


static void filter_postprocess(MSFilter *f) {
}

static void filter_unit(MSFilter *f) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    if (obj->file)
        fclose(obj->file);
    ms_free(f->data);
}


/*filter specific method*/

static int filter_set_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    obj->sampleRate = *(int32_t *) arg;
    ms_message("Lyra decoder - sample rate set to %d", obj->sampleRate);

    return 0;
}

static int filter_get_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    *(int *) arg = obj->sampleRate;
    return 0;
}

static int filter_get_nchannels(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_dec_struct *) f->data;
    *(int *) arg = obj->nchannels;
    return 0;
}

static int filter_set_rtp_picker(MSFilter *f, void *arg) {
//	struct lyra_dec_struct* obj= (struct lyra_dec_struct*) f->data;
//	obj->rtp_picker_context=*(MSRtpPayloadPickerContext*)arg;
    return 0;
}

static int filter_have_plc(MSFilter *f, void *arg) {
    *((int *) arg) = 1;
    return 0;
}

static MSFilterMethod filter_methods[] = {
        {MS_FILTER_SET_SAMPLE_RATE,        filter_set_sample_rate},
        {MS_FILTER_GET_SAMPLE_RATE,        filter_get_sample_rate},
        {MS_FILTER_GET_NCHANNELS,          filter_get_nchannels},
        {MS_FILTER_SET_RTP_PAYLOAD_PICKER, filter_set_rtp_picker},
        {MS_DECODER_HAVE_PLC,              filter_have_plc},
        {0, NULL}
};


MSFilterDesc ms_lyra_3200_dec_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSLYRADec",
        .text="Lyra decoder filter.",
        .category=MS_FILTER_DECODER,
        .enc_fmt="LYRA3200",
        .ninputs=1, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_unit,
        .methods=filter_methods,
        .flags=MS_FILTER_IS_PUMP
};
MSFilterDesc ms_lyra_6000_dec_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSLYRADec",
        .text="Lyra decoder filter.",
        .category=MS_FILTER_DECODER,
        .enc_fmt="LYRA6000",
        .ninputs=1, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_unit,
        .methods=filter_methods,
        .flags=MS_FILTER_IS_PUMP
};
MSFilterDesc ms_lyra_9200_dec_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSLYRADec",
        .text="Lyra decoder filter.",
        .category=MS_FILTER_DECODER,
        .enc_fmt="LYRA9200",
        .ninputs=1, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_unit,
        .methods=filter_methods,
        .flags=MS_FILTER_IS_PUMP
};
MS_FILTER_DESC_EXPORT (ms_lyra_3200_dec_desc)
MS_FILTER_DESC_EXPORT (ms_lyra_6000_dec_desc)
MS_FILTER_DESC_EXPORT (ms_lyra_9200_dec_desc)