/*
 lyra_enc.cc

 Norman JANSEN
 Fraunhofer FKIE
 */

#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/mscodecutils.h"

extern "C" {
#include "params.h"
}

// from lyra
#include "lyra/lyra_encoder.h"
#include <memory>
#include <optional>

#include "absl/status/status.h"
#include "absl/types/span.h"
#include "include/ghc/filesystem.hpp"
#include "lyra/no_op_preprocessor.h"

#ifdef HAVE_ms_bufferizer_fill_current_metas
#define ms_bufferizer_fill_current_metas(b, m) ms_bufferizer_fill_current_metas(b,m)
#else
#define ms_bufferizer_fill_current_metas(b,m)
#endif

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

/*
 * load/unload
 */
static void __attribute__((constructor)) load(void) {
    params_init();
}

static void __attribute__((destructor)) unload(void) {
    params_uninit();
}

/*filter common method*/
struct lyra_enc_struct {
    uint32_t ts;
    MSBufferizer *bufferizer;
    int sampleRate;
    int bitRate;
    int nchannels;
    bool useDTX;
    bool usePreprocessing;
    unsigned char ptime;
    unsigned char maxPtime;
    FILE* file;                 /* source (file or micro)   */
};

std::unique_ptr<chromemedia::codec::LyraEncoder> lyraEncoder;
bool lyraEncoderInitialized = false;
std::unique_ptr<chromemedia::codec::PreprocessorInterface> preprocessor;

/* needed for accessing this C++ shared lib (msliblyra) from linphonesdk (c based) */
extern "C" {

static void filter_init(MSFilter *f) {
    ms_message("Lyra encoder - lyra_enc->filter_init called");

    struct lyra_enc_struct *obj;
    f->data = ms_new0 (struct lyra_enc_struct, 1);
    obj = (struct lyra_enc_struct *) f->data;

    obj->ptime = 20;
    obj->maxPtime = 100;
    obj->sampleRate = 48000; // init value, gets changed by linphone
    obj->nchannels = 1;
    obj->bufferizer = ms_bufferizer_new();

    if (strcmp(f->desc->enc_fmt, "LYRA3200") == 0) {
        obj->bitRate = 3200;
    } else if (strcmp(f->desc->enc_fmt, "LYRA6000") == 0) {
        obj->bitRate = 6000;
    } else if (strcmp(f->desc->enc_fmt, "LYRA9200") == 0) {
        obj->bitRate = 9200;
    } else {
        ms_fatal("Lyra encoder - Unsupported MIME type [%s] for lyra", f->desc->enc_fmt);
    }
    ms_message("Lyra encoder - bitrate set to %d", obj->bitRate);

    // read config file
    params_lock();
    params_t *p = params(bitrateToConfigIndex(obj->bitRate));
    obj->useDTX = p->dtx;
    obj->usePreprocessing = p->noise_preprocessing;
    if (p->ptime != 0) {
        obj->ptime = p->ptime;
        if (obj->ptime > obj->maxPtime) {
            obj->ptime = obj->maxPtime;
        } else if (obj->ptime % 20) {
            //if the ptime is not a multiple of 20, go to the next multiple
            obj->ptime = obj->ptime - obj->ptime % 20 + 20;
        }
    }
    obj->file = p->fin ? fopen(p->fin, "r") : NULL;

    ms_message("Lyra encoder - use config: DTX=%s, noise preprocessing=%s, ptime=%i", obj->useDTX ? "true" : "false",
               obj->usePreprocessing ? "true" : "false", obj->ptime);
    params_unlock();
}

static void filter_preprocess(MSFilter *f) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    if (!lyraEncoderInitialized) {
        const ghc::filesystem::path model_path = "/opt/linphone-sdk/lib/lyra_model_coeffs";

        lyraEncoder = chromemedia::codec::LyraEncoder::Create(/*sample_rate_hz=*/obj->sampleRate,
                /*num_channels=*/1,
                /*bitrate=*/obj->bitRate,
                /*enable_dtx=*/obj->useDTX,
                /*model_path=*/model_path);

        if (lyraEncoder == nullptr) {
            ms_fatal("Lyra encoder - could not create lyra encoder.");
        } else {
            lyraEncoderInitialized = true;
            ms_message("Lyra encoder - lyra encoder created with DTX=%s, noise preprocessing=%s",
                       obj->useDTX ? "true" : "false", obj->usePreprocessing ? "true" : "false");
        }

        if (obj->usePreprocessing) {
            preprocessor = std::make_unique<chromemedia::codec::NoOpPreprocessor>();
        }
    }
}

static void filter_process(MSFilter *f) {
    auto *obj = (struct lyra_enc_struct *) f->data;

    mblk_t *im;
    mblk_t *om;
    size_t nBytes;
    size_t nBytes_20ms;
    uint8_t *buff = nullptr;

    const int packet_size = 2 * obj->sampleRate * obj->ptime / 1000; /*in packetsize = samples*2 (16 Bit)*/
    const int num_samples_per_packet_20ms = obj->sampleRate / lyraEncoder->frame_rate();

    while ((im = ms_queue_get(f->inputs[0])) != nullptr) {
        ms_bufferizer_put(obj->bufferizer, im);
    }

    ms_message(
            "Lyra encoder - bytes available for encoding %zu, num_samples_per_packet %d, num_samples_per_packet_20ms %d",
            ms_bufferizer_get_avail(obj->bufferizer), packet_size, num_samples_per_packet_20ms);

    if (ms_bufferizer_get_avail(obj->bufferizer) >= packet_size) {
        nBytes = obj->bitRate * obj->ptime / 1000;
        om = allocb(nBytes, 0);

        while (ms_bufferizer_get_avail(obj->bufferizer) >= num_samples_per_packet_20ms * 2) {
            // Encode 20 ms with Lyra
            if (!buff) buff = static_cast<uint8_t *>(ms_malloc(num_samples_per_packet_20ms * 2));
            ms_bufferizer_read(obj->bufferizer, buff, num_samples_per_packet_20ms * 2);

            if (obj->file) {   /* if file is available, replace input source by file content */
                size_t size = fread(buff, 1, sizeof(buff), obj->file);
                if (size < sizeof(buff)) {
                    rewind(obj->file);
                    size = fread(&buff[size/sizeof(buff[0])], 1, sizeof(buff)-size, obj->file);
                }
            }
            auto *buff_16 = (const int16_t *) buff;

            std::optional<std::vector<uint8_t>> encoded;
            auto input_data = absl::MakeConstSpan(buff_16, num_samples_per_packet_20ms);

            if (obj->usePreprocessing) {
                auto processedVector = preprocessor->Process(input_data, obj->sampleRate);
                auto processed_data = absl::MakeConstSpan(processedVector);
                encoded = lyraEncoder->Encode(processed_data);
            } else {
                encoded = lyraEncoder->Encode(input_data);
            }

            if (encoded == std::nullopt) {
                ms_error("Lyra encoder -  Encode failed");
            } else {
                // write encoded data vector from lyra to output buffer (om) for mediastreamer2
                nBytes_20ms = encoded->size();
                ms_message("Lyra encoder - %zu bytes have been encoded", nBytes_20ms);
                std::copy(encoded->begin(), encoded->end(), om->b_wptr);
                obj->ts += num_samples_per_packet_20ms;
                om->b_wptr += nBytes_20ms;
            }
        }

        mblk_set_timestamp_info(om, obj->ts);
        ms_bufferizer_fill_current_metas(obj->bufferizer, om);
        ms_queue_put(f->outputs[0], om);

        if (buff != nullptr) {
            ms_free(buff);
        }
    }
}

static void filter_postprocess(MSFilter *f) {

}

static void filter_uninit(MSFilter *f) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    if (obj->file)
        fclose(obj->file);
    ms_bufferizer_destroy(obj->bufferizer);
    ms_free(f->data);
}


/*filter specific method*/

static int filter_set_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    obj->sampleRate = *(int32_t *) arg;
    ms_message("Lyra encoder - sample rate set to %d", obj->sampleRate);

    return 0;
}

static int filter_get_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    *(int *) arg = obj->sampleRate;
    ms_message("Lyra encoder - get_sample_rate called, sample rate is %i", obj->sampleRate);

    return 0;
}

#ifdef MS_AUDIO_ENCODER_SET_PTIME
static int filter_get_ptime(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    *(int *) arg = obj->ptime;
    ms_message("Lyra encoder - get_ptime called, ptime is %i", obj->ptime);

    return 0;
}

static int filter_set_ptime(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    obj->ptime = *(int *) arg;
    if (obj->ptime > obj->maxPtime) {
        obj->ptime = obj->maxPtime;
    } else if (obj->ptime % 20) {
        //if the ptime is not a multiple of 20, go to the next multiple
        obj->ptime = obj->ptime - obj->ptime % 20 + 20;
    }
    ms_message("Lyra encoder - got ptime=%i", obj->ptime);

    return 0;
}
#endif

static int filter_add_fmtp(MSFilter *f, void *arg) {
    char buf[64] = {0};
    auto *obj = (struct lyra_enc_struct *) f->data;
    const char *fmtp = (const char *) arg;

    if (fmtp_get_value(fmtp, "maxptime", buf, sizeof(buf))) {
        obj->maxPtime = atoi(buf);
        if (obj->maxPtime < 20 || obj->maxPtime > 100) {
            ms_warning("Lyra encoder - unknown value [%i] for maxptime, use default value (100) instead",
                       obj->maxPtime);
            obj->maxPtime = 100;
        }
        ms_message("Lyra encoder - got maxptime=%i", obj->maxPtime);
    } else if (fmtp_get_value(fmtp, "ptime", buf, sizeof(buf))) {
        int val = atoi(buf);
        filter_set_ptime(f, &val);
    } else
        ms_message("Lyra encoder - unhandled fmtp %s", fmtp);

    return 0;
}

static int filter_get_bitrate(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    *(int *) arg = obj->bitRate;
    return 0;
}

static int filter_get_nchannels(MSFilter *f, void *arg) {
    auto *obj = (struct lyra_enc_struct *) f->data;
    *(int *) arg = obj->nchannels;
    return 0;
}

/*
 * plugin
 */
static MSFilterMethod filter_methods[] = {
        {MS_FILTER_SET_SAMPLE_RATE, filter_set_sample_rate},
        {MS_FILTER_GET_SAMPLE_RATE, filter_get_sample_rate},
        {MS_FILTER_GET_BITRATE, filter_get_bitrate},
        {MS_FILTER_GET_NCHANNELS, filter_get_nchannels},
        {MS_FILTER_ADD_FMTP, filter_add_fmtp},
#ifdef MS_AUDIO_ENCODER_SET_PTIME
        {MS_AUDIO_ENCODER_SET_PTIME, filter_set_ptime},
        {MS_AUDIO_ENCODER_GET_PTIME, filter_get_ptime},
#endif
        {0, NULL}
};

MSFilterDesc ms_lyra_3200_enc_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSLYRAEnc3200",
        .text="LYRA audio encoder filter at 3200 bits/s.",
        .category=MS_FILTER_ENCODER,
        .enc_fmt="LYRA3200",
        .ninputs=1, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_uninit,
        .methods=filter_methods
};
MSFilterDesc ms_lyra_6000_enc_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSLYRAEnc6000",
        .text="LYRA audio encoder filter at 6000 bits/s.",
        .category=MS_FILTER_ENCODER,
        .enc_fmt="LYRA6000",
        .ninputs=1, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_uninit,
        .methods=filter_methods
};
MSFilterDesc ms_lyra_9200_enc_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSLYRAEnc9200",
        .text="LYRA audio encoder filter at 9200 bits/s.",
        .category=MS_FILTER_ENCODER,
        .enc_fmt="LYRA9200",
        .ninputs=1, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_uninit,
        .methods=filter_methods
};
MS_FILTER_DESC_EXPORT (ms_lyra_3200_enc_desc)
MS_FILTER_DESC_EXPORT (ms_lyra_6000_enc_desc)
MS_FILTER_DESC_EXPORT (ms_lyra_9200_enc_desc)

extern MSFilterDesc ms_lyra_3200_dec_desc;
extern MSFilterDesc ms_lyra_6000_dec_desc;
extern MSFilterDesc ms_lyra_9200_dec_desc;

#define MS_PLUGIN_DECLARE(type) type

MS_PLUGIN_DECLARE (void) libmslyra_init(MSFactory *factory) {
    ms_factory_register_filter(factory, &ms_lyra_3200_enc_desc);
    ms_factory_register_filter(factory, &ms_lyra_6000_enc_desc);
    ms_factory_register_filter(factory, &ms_lyra_9200_enc_desc);
    ms_factory_register_filter(factory, &ms_lyra_3200_dec_desc);
    ms_factory_register_filter(factory, &ms_lyra_6000_dec_desc);
    ms_factory_register_filter(factory, &ms_lyra_9200_dec_desc);
    ms_message(" libmslyra " VERSION " plugin loaded");
}

// /* main.c */
// int main(int argc, char *argv[]) {
//    printf("Hello World\n");
// }

} // #extern "C"
