/*
 speechtext_dec.cc

 Norman JANSEN
 Fraunhofer FKIE
 */

extern "C" {
#include "speechtotext/client/include/apiClient.h"
#include "speechtotext/client/api/DefaultAPI.h"
#include "texttospeech/client/include/apiClient.h"
#include "texttospeech/client/api/DefaultAPI.h"
#include "mqtttransport/client/include/apiClient.h"
#include "mqtttransport/client/api/DefaultAPI.h"
}

#include "mediastreamer2/msfilter.h"
#include "ortp/rtp.h"
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "texttospeech/server/include/texttospeech/MessageBuffer.h"
#include "mqtttransport/server/include/mqtttransport/MessageBuffer.h"
#include <cstdint>
#include <filesystem>
#include <string>
#include "central_logger.h"

extern "C" {
#include "params.h"
}

/*filter common method*/
struct speechtext_dec_struct {
    int sampleRate;
    int nchannels;
    unsigned int ptime;
    unsigned int audio_chunk_size;
    std::string url_central_logger;
    std::string url_speech_to_text_webhook;
    std::string url_text_to_speech_webhook;
    std::string url_mqtt_transport_webhook;
    texttospeech_apiClient_t *apiClient_texttospeech;
    FILE *file;     /* sink (optional)           */
};

std::string convertBytesToHex(std::uint8_t *buf, size_t length) {
    std::ostringstream hexStream;

    // Set the stream to format the output as hexadecimal with a width of 2 characters and fill with '0'
    hexStream << std::hex << std::setfill('0');

    // Convert each byte to its hexadecimal representation
    for (size_t i = 0; i < length; ++i) {
        hexStream << std::setw(2) << static_cast<int>(buf[i]);
    }

    return hexStream.str();
}


static void filter_init(MSFilter *f) {
    ms_message("Speechtext decoder - speech_dec->filter_init called");

    f->data = ms_new0(struct speechtext_dec_struct, 1);
    auto *obj = (struct speechtext_dec_struct *) f->data;

    obj->nchannels = 1;
    obj->ptime = 20;
    obj->audio_chunk_size = 3200;

    params_lock();
    params_t *p = params();
    if (p->ptime != 0) {
        obj->ptime = p->ptime;
        if (obj->ptime % 20) {
            //if the ptime is not a multiple of 20, go to the next multiple
            obj->ptime = obj->ptime - obj->ptime % 20 + 20;
        }
    }
    if (p->audio_chunk_size != 0) {
        obj->audio_chunk_size = p->audio_chunk_size;
    }
    obj->url_central_logger = p->url_central_logger;
    obj->url_speech_to_text_webhook = p->url_speech_to_text_webhook;
    obj->url_speech_to_text_webhook = p->url_text_to_speech_webhook;
    obj->url_mqtt_transport_webhook = p->url_mqtt_transport_webhook;
    obj->file = p->fout ? fopen(p->fout, "w") : NULL;
    params_unlock();

    ms_message(
            "Speechtext decoder - use config: "
            "ptime=%i, "
            "audio_chunk_size=%i, sampleRate=%i, nchannels=%i, "
            "url_central_logger=%s, "
            "url_speech_to_text_webhook=%s, "
            "url_mqtt_transport_webhook=%s",
            obj->ptime,
            obj->audio_chunk_size,
            obj->sampleRate,
            obj->nchannels,
            obj->url_central_logger.data(),
            obj->url_speech_to_text_webhook.data(),
            obj->url_mqtt_transport_webhook.data()
    );

    // create OpenAPI-Clients
    obj->apiClient_texttospeech = texttospeech_apiClient_create();
}

static void filter_preprocess(MSFilter *f) {
    auto *obj = (struct speechtext_dec_struct *) f->data;
    std::filesystem::path cwd = std::filesystem::current_path();
    ms_message("Speechtext decoder - current path: %s", cwd.c_str());
}

static void filter_process(MSFilter *f) {
    mblk_t *im;
    auto *obj = (struct speechtext_dec_struct *) f->data;
    uint8_t *buffer;

    // get text messages from Mqtttransport buffer
    std::optional<org::openapitools::mqtttransport::server::api::mqtt_message> msg_mqtt = org::openapitools::mqtttransport::server::api::MessageBuffer::getInstance().get_message();
    while (msg_mqtt != std::nullopt) {

        // log received event to central logger (vector)
        logger->sendLog(obj->url_central_logger, msg_mqtt->sender_id, CentralLogger::LoggingEventType::Received,
                        CentralLogger::current_epoch_ms(), msg_mqtt->sender_id, {msg_mqtt->receiver_id},
                        msg_mqtt->message_id, "", msg_mqtt->message);

        // call Texttospeech
        ms_message("send to TTS: %s", msg_mqtt->message.data());
        auto provide_text_request = provide_text_request_create(msg_mqtt->timestamp, CentralLogger::current_epoch_ms(),
                                                                msg_mqtt->message_id.data(), msg_mqtt->message.data());
        Texttospeech_provideText(obj->apiClient_texttospeech, provide_text_request);

        ms_message("sent to TTS");

        msg_mqtt = org::openapitools::mqtttransport::server::api::MessageBuffer::getInstance().get_message();
    }

    // Get audio from the Text-to-Speech buffer
    std::optional<org::openapitools::texttospeech::server::api::audio_post> msg_audio = org::openapitools::texttospeech::server::api::MessageBuffer::getInstance().get_message();
    while (msg_audio != std::nullopt) {

        // log processed event (i.e. start of playing the audio) to central logger (vector)
        logger->sendLog(obj->url_central_logger, "", CentralLogger::LoggingEventType::Processed,
                        CentralLogger::current_epoch_ms(), "", {""},
                        msg_audio->message_id, "", "");

        int totalBytesToDecode = msg_audio->length; // Length of the decoded audio in bytes
        int audio_chunk_size = obj->audio_chunk_size;
        std::string base64String = msg_audio->byte_array;
        ms_message("Speechtext decoder - got audio of %d bytes.", msg_audio->length);

        // Decode base64 in chunks
        BIO *bio, *b64;
        b64 = BIO_new(BIO_f_base64());
        bio = BIO_new_mem_buf(base64String.data(), -1);
        bio = BIO_push(b64, bio);

        // Read from BIO with no newlines
        BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

        int totalDecoded = 0;
        while (true) {
            // Calculate remaining bytes to decode
            int remainingBytesToDecode = totalBytesToDecode - totalDecoded;

            // Check if all bytes have been decoded
            if (remainingBytesToDecode == 0) {
                break; // all bytes decoded
            }

            // Decode a chunk
            int chunkSize = std::min(audio_chunk_size, remainingBytesToDecode);
            // Allocate memory for the decoded bytes
            mblk_t *om = allocb(chunkSize, 0);
            if (!om) {
                ms_error("Error allocating memory for a block.");
            }
            buffer = (uint8_t *) om->b_wptr;
            int bytesRead = BIO_read(bio, buffer, chunkSize);

            if (bytesRead > 0) {
                totalDecoded += bytesRead;
                printf("%s", convertBytesToHex(om->b_wptr, bytesRead).data());
                // Update the buffer pointer
                om->b_wptr += bytesRead;

                // Add the block to the queue
                ms_queue_put(f->outputs[0], om);

                // If a file is open, write the block to the file
                if (obj->file) {
                    fwrite(buffer, 1, bytesRead, obj->file);
                }
            } else {
                break; // Exit when no more data to read
            }
        }
        printf("\n");

        BIO_free_all(bio);

        ms_message("Speechtext decoder - decoded output of length %d written to output buffer", msg_audio->length);
        msg_audio = org::openapitools::texttospeech::server::api::MessageBuffer::getInstance().get_message();
    }
}


static void filter_postprocess(MSFilter *f) {
}

static void filter_unit(MSFilter *f) {
    auto *obj = (struct speechtext_dec_struct *) f->data;
    ms_free(f->data);
}


/*filter specific method*/

static int filter_set_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_dec_struct *) f->data;
    obj->sampleRate = *(int32_t *) arg;
    ms_message("Speechtext decoder - sample rate set to %d", obj->sampleRate);

    return 0;
}

static int filter_get_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_dec_struct *) f->data;
    *(int *) arg = obj->sampleRate;
    return 0;
}

static int filter_get_nchannels(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_dec_struct *) f->data;
    *(int *) arg = obj->nchannels;
    return 0;
}

static int filter_set_rtp_picker(MSFilter *f, void *arg) {
//	struct speech_dec_struct* obj= (struct speech_dec_struct*) f->data;
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


MSFilterDesc ms_speechtext_dec_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSSPEECHTEXTDec",
        .text="Speechtext decoder filter.",
        .category=MS_FILTER_DECODER,
        .enc_fmt="SPEECHTEXT",
        .ninputs=0, /*number of inputs*/
        .noutputs=1, /*number of outputs*/
        .init=filter_init,
        .preprocess=filter_preprocess,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_unit,
        .methods=filter_methods,
        .flags=MS_FILTER_IS_PUMP
};
MS_FILTER_DESC_EXPORT (ms_speechtext_dec_desc)
