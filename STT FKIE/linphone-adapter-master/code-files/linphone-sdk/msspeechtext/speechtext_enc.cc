/*
 speechtext_enc.cc

 Norman JANSEN
 Fraunhofer FKIE
 */

#include "speechtotext/server/include/speechtotext/MessageBuffer.h"
#include <thread>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include "mediastreamer2/msfilter.h"
#include "mediastreamer2/mscodecutils.h"
#include <ifaddrs.h>
#include <unistd.h>
#include "central_logger.h"

extern "C" {
#include "params.h"
#include "speechtotext/client/include/apiClient.h"
#include "speechtotext/client/api/DefaultAPI.h"
#include "texttospeech/client/include/apiClient.h"
#include "texttospeech/client/api/DefaultAPI.h"
#include "mqtttransport/client/include/apiClient.h"
#include "mqtttransport/client/api/DefaultAPI.h"
}

#ifdef HAVE_ms_bufferizer_fill_current_metas
#define ms_bufferizer_fill_current_metas(b, m) ms_bufferizer_fill_current_metas(b,m)
#else
#define ms_bufferizer_fill_current_metas(b,m)
#endif

CentralLogger *logger = CentralLogger::getInstance();

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
struct speechtext_enc_struct {
    uint32_t ts;
    MSBufferizer *bufferizer;
    int sampleRate;
    int nchannels;
    std::string dil_interface_name;
    unsigned int ptime;
    unsigned int audio_chunk_size;
    std::string url_central_logger;
    std::string url_speech_to_text_webhook;
    std::string url_text_to_speech_webhook;
    std::string url_mqtt_transport_webhook;
    std::string speechtotext_config_profile;
    std::string texttospeech_config_profile;
    std::string speechtotext_variant;
    std::string texttospeech_variant;
    std::string mqtt_variant;
    std::string mqtt_server_ip;
    int mqtt_server_port;
    std::string mqtt_source_ip;
    std::string mqtt_destination_ip;
    std::string mqtt_sender_id;
    std::string mqtt_receiver_id;
    std::string mqtt_config_profile;
    std::string mqtt_base_topic;
    std::string mqtt_node_id;
    int message_id;
    speechtotext_apiClient_t *apiClient_speechtotext;
    texttospeech_apiClient_t *apiClient_texttospeech;
    mqtttransport_apiClient_t *apiClient_mqtttransport;
};

// Function to encode uint8_t buffer to Base64
std::string base64EncodeFromByteBuffer(const uint8_t *buffer, size_t length) {
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    // Write to BIO with no newlines
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(bio, buffer, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string base64String(bufferPtr->data, bufferPtr->length);
    BIO_free_all(bio);

    return base64String;
}

std::string getInterfaceIPAddress(const std::string &interfaceName) {
    struct ifaddrs *ifaddr, *ifa;
    std::string ipAddress = "Unknown";

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs failed");
        return ipAddress;
    }

    for (ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr || ifa->ifa_addr->sa_family != AF_INET) {
            continue;
        }

        // Check if the interface name starts with the given prefix
        std::string currentInterfaceName(ifa->ifa_name);
        if (currentInterfaceName.find(interfaceName) == 0) { // Matches prefix
            char ip[INET_ADDRSTRLEN];
            struct sockaddr_in *addr = (struct sockaddr_in *) ifa->ifa_addr;

            if (inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip)) != nullptr) {
                ipAddress = ip;
                break; // Stop once we find the first match
            }
        }
    }

    freeifaddrs(ifaddr);
    return ipAddress;
}

int getLastOctet(const std::string &ipAddress) {
    std::stringstream ss(ipAddress);
    std::string token;
    std::vector<std::string> parts;

    // Split the IP address by the '.' delimiter
    while (std::getline(ss, token, '.')) {
        parts.push_back(token);
    }

    // Check if the IP address has 4 parts
    if (parts.size() == 4) {
        return std::stoi(parts.back());  // Convert the last part to an integer
    } else {
        std::cerr << "Invalid IP address format!" << std::endl;
        return -1;  // Return an error code if the format is invalid
    }
}

// Helper function to replace "{ip}" with the IP address in a given URL string
void replaceInUrl(std::string &url, const std::string &ipAddress) {
    size_t pos = 0;
    // Replace all occurrences of "{ip}" with ipAddress
    while ((pos = url.find("{ip}", pos)) != std::string::npos) {
        url.replace(pos, 4, ipAddress); // 4 is the length of "{ip}"
        pos += ipAddress.length(); // Move past the replaced part
    }
}

// Initialization functions for REST server
void initialize_speechtotext(struct speechtext_enc_struct *obj) {
    ms_message("Speechtext encoder - initialize STT");

    speechtotext_initialize_speechtotext_variant_e variant;

    if (strcmp(obj->speechtotext_variant.c_str(), "Whisper") == 0) {
        variant = speechtotext_initialize_SPEECHTOTEXTVARIANT_Whisper;
    } else if (strcmp(obj->speechtotext_variant.c_str(), "Vosk") == 0) {
        variant = speechtotext_initialize_SPEECHTOTEXTVARIANT_Vosk;
    } else {
        variant = speechtotext_initialize_SPEECHTOTEXTVARIANT_NULL; // Default case
    }

    Speechtotext_initialize(
            obj->apiClient_speechtotext,
            variant,
            obj->speechtotext_config_profile.data(),
            obj->url_speech_to_text_webhook.data());
}

void initialize_texttospeech(struct speechtext_enc_struct *obj) {
    ms_message("Speechtext encoder - initialize TTS");

    texttospeech_initialize_texttospeech_variant_e variant;

    if (strcmp(obj->texttospeech_variant.c_str(), "standard") == 0) {
        variant = texttospeech_initialize_TEXTTOSPEECHVARIANT_standard;
    } else if (strcmp(obj->texttospeech_variant.c_str(), "coqui_yourtts") == 0) {
        variant = texttospeech_initialize_TEXTTOSPEECHVARIANT_coqui_yourtts;
    } else if (strcmp(obj->texttospeech_variant.c_str(), "coqui_xtts") == 0) {
        variant = texttospeech_initialize_TEXTTOSPEECHVARIANT_coqui_xtts;
    } else {
        variant = texttospeech_initialize_TEXTTOSPEECHVARIANT_NULL; // Default case
    }

    Texttospeech_initialize(
            obj->apiClient_texttospeech,
            variant,
            obj->texttospeech_config_profile.data(),
            obj->url_text_to_speech_webhook.data());
}

void initialize_mqtt(struct speechtext_enc_struct *obj) {
    ms_message("Speechtext encoder - initialize MQTT");

    mqtttransport_initialize_mqtt_variant_e variant;

    if (strcmp(obj->mqtt_variant.c_str(), "Standard") == 0) {
        variant = mqtttransport_initialize_MQTTVARIANT_Standard;
    } else if (strcmp(obj->texttospeech_variant.c_str(), "MQTT-UDP") == 0) {
        variant = mqtttransport_initialize_MQTTVARIANT_MQTT_UDP;
    } else {
        variant = mqtttransport_initialize_MQTTVARIANT_NULL; // Default case
    }

    Mqtttransport_initialize(
            obj->apiClient_mqtttransport,
            variant,
            obj->mqtt_config_profile.data(),
            obj->mqtt_base_topic.data(),
            obj->mqtt_node_id.data(),
            obj->url_mqtt_transport_webhook.data(),
            obj->mqtt_server_ip.data(),
            &obj->mqtt_server_port);
}

/* needed for accessing this C++ shared lib (mslibspeechtext) from linphonesdk (c based) */
extern "C" {

static void filter_init(MSFilter *f) {
    ms_message("Speechtext encoder - speechtext_enc->filter_init called");

    struct speechtext_enc_struct *obj;
    f->data = ms_new0 (struct speechtext_enc_struct, 1);
    obj = (struct speechtext_enc_struct *) f->data;

    obj->ptime = 20;
    obj->audio_chunk_size = 3200;
    obj->sampleRate = 48000; // init value, gets changed by linphone
    obj->nchannels = 1;
    obj->bufferizer = ms_bufferizer_new();
    obj->message_id = 0;

    // read config file
    params_lock();
    params_t *p = params();
    obj->dil_interface_name = p->dil_interface_name;
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
    obj->url_text_to_speech_webhook = p->url_text_to_speech_webhook;
    obj->url_mqtt_transport_webhook = p->url_mqtt_transport_webhook;
    obj->speechtotext_config_profile = p->speechtotext_config_profile;
    obj->texttospeech_config_profile = p->texttospeech_config_profile;
    obj->speechtotext_variant = p->speechtotext_variant;
    obj->texttospeech_variant = p->texttospeech_variant;
    obj->mqtt_variant = p->mqtt_variant;
    obj->mqtt_server_ip = p->mqtt_server_ip;
    obj->mqtt_server_port = p->mqtt_server_port;
    obj->mqtt_source_ip = p->mqtt_source_ip;
    obj->mqtt_destination_ip = p->mqtt_destination_ip;
    obj->mqtt_sender_id = p->mqtt_sender_id;
    obj->mqtt_receiver_id = p->mqtt_receiver_id;
    obj->mqtt_config_profile = p->mqtt_config_profile;
    obj->mqtt_base_topic = p->mqtt_base_topic;
    obj->mqtt_node_id = p->mqtt_node_id;
    params_unlock();

    // take last octet from IP address and append this to the node_id to create a unique node_id
    std::string ipAddress = getInterfaceIPAddress(obj->dil_interface_name);
    obj->mqtt_node_id = obj->mqtt_node_id + std::to_string(getLastOctet(ipAddress));

    // replace {ip} in URLs with IP address
    replaceInUrl(obj->url_speech_to_text_webhook, ipAddress);
    replaceInUrl(obj->url_text_to_speech_webhook, ipAddress);
    replaceInUrl(obj->url_mqtt_transport_webhook, ipAddress);

    ms_message(
            "Speechtext encoder - use config: "
            "ptime=%i, mqtt_node_id=%s, "
            "audio_chunk_size=%i, sampleRate=%i, nchannels=%i, "
            "dil_interface_name=%s, "
            "url_central_logger=%s, "
            "url_speech_to_text_webhook=%s, "
            "url_text_to_speech_webhook=%s, "
            "url_mqtt_transport_webhook=%s, "
            "speechtotext_config_profile=%s, "
            "texttospeech_config_profile=%s, "
            "speechtotext_variant=%s, "
            "texttospeech_variant=%s, "
            "mqtt_variant=%s, "
            "mqtt_server_ip=%s, mqtt_server_port=%i, "
            "mqtt_source_ip=%s, mqtt_destination_ip=%s, "
            "mqtt_sender_id=%s, mqtt_receiver_id=%s, "
            "mqtt_config_profile=%s, mqtt_base_topic=%s",
            obj->ptime,
            obj->mqtt_node_id.data(),
            obj->audio_chunk_size,
            obj->sampleRate,
            obj->nchannels,
            obj->dil_interface_name.data(),
            obj->url_central_logger.data(),
            obj->url_speech_to_text_webhook.data(),
            obj->url_text_to_speech_webhook.data(),
            obj->url_mqtt_transport_webhook.data(),
            obj->speechtotext_config_profile.data(),
            obj->texttospeech_config_profile.data(),
            obj->speechtotext_variant.data(),
            obj->texttospeech_variant.data(),
            obj->mqtt_variant.data(),
            obj->mqtt_server_ip.data(),
            obj->mqtt_server_port,
            obj->mqtt_source_ip.data(),
            obj->mqtt_destination_ip.data(),
            obj->mqtt_sender_id.data(),
            obj->mqtt_receiver_id.data(),
            obj->mqtt_config_profile.data(),
            obj->mqtt_base_topic.data()
    );

    // create OpenAPI-Clients
    obj->apiClient_speechtotext = speechtotext_apiClient_create();
    obj->apiClient_texttospeech = texttospeech_apiClient_create();
    obj->apiClient_mqtttransport = mqtttransport_apiClient_create();

    // todo make STT variant configurable

    // Launch each initialization in its own thread.
    std::thread threadSTT(initialize_speechtotext, obj);
    std::thread threadTTS(initialize_texttospeech, obj);
    std::thread threadMQTT(initialize_mqtt, obj);

    // Wait for all threads to finish.
    threadSTT.join();
    threadTTS.join();
    threadMQTT.join();

    ms_message("Speechtext encoder - STT, TTS and MQTT webhook server initialized");
}

static void filter_process(MSFilter *f) {
    auto *obj = (struct speechtext_enc_struct *) f->data;

    mblk_t *im;
    uint8_t *buff = nullptr;

    const int packet_size = 2 * obj->sampleRate * obj->ptime / 1000; /*in packetsize = samples*2 (16 Bit)*/

    while ((im = ms_queue_get(f->inputs[0])) != nullptr) {
        ms_bufferizer_put(obj->bufferizer, im);
    }

    ms_message("Speechtext encoder - audio bytes available %zu, packet_size %d",
               ms_bufferizer_get_avail(obj->bufferizer), packet_size);

    // send audio to SpeechtoText
    if (ms_bufferizer_get_avail(obj->bufferizer) >= packet_size) {
        if (!buff) buff = static_cast<uint8_t *>(ms_malloc(packet_size));
        ms_bufferizer_read(obj->bufferizer, buff, packet_size);

        // send audio to SpeechtoText
        std::string base64_str = base64EncodeFromByteBuffer(buff, packet_size);

        auto provide_audio_request = provide_audio_request_create(packet_size,
                                                                  obj->sampleRate,
                                                                  CentralLogger::current_epoch_ms(),
                                                                  base64_str.data());
        std::string ipAddress = getInterfaceIPAddress(obj->dil_interface_name);
        ms_message("(ip=%s): send to STT: %s", ipAddress.c_str(), base64_str.c_str());
        Speechtotext_provideAudio(obj->apiClient_speechtotext, provide_audio_request);
        ms_message("sent to STT");

        if (buff != nullptr) {
            ms_free(buff);
        }
    }

    // get text from Speechtotext buffer
    std::optional<org::openapitools::speechtotext::server::api::text_message> msg_text = org::openapitools::speechtotext::server::api::MessageBuffer::getInstance().get_message();
    while (msg_text != std::nullopt) {
        ms_message("Speechtext encoder - got message from Speechtotext buffer: %s", msg_text->text.data());

        // send text to Mqtttransport
        ms_message("send to MQTT");
        std::string sender_id = obj->mqtt_sender_id;
        std::string receiver_id = obj->mqtt_receiver_id;
        obj->message_id++;
        std::string message_id = obj->mqtt_sender_id + "#" + std::to_string(obj->message_id);
        std::string source_ip = obj->mqtt_source_ip;
        std::string destination_ip = obj->mqtt_destination_ip;

        // log message to central logger (vector)
        ms_message("Norman 0");
        logger->sendLog(obj->url_central_logger, sender_id, CentralLogger::LoggingEventType::Triggered,
                        msg_text->timestamp_end, sender_id, {receiver_id},
                        message_id, "", msg_text->text);
        logger->sendLog(obj->url_central_logger, sender_id, CentralLogger::LoggingEventType::Sent,
                        CentralLogger::current_epoch_ms(), sender_id, {receiver_id},
                        message_id, "", msg_text->text);

        auto message = message_create(sender_id.data(), receiver_id.data(), message_id.data(), msg_text->text.data(),
                                      CentralLogger::current_epoch_ms());
        Mqtttransport_sendMessage(obj->apiClient_mqtttransport, source_ip.data(), destination_ip.data(), message);
        ms_message("sent to MQTT");

        msg_text = org::openapitools::speechtotext::server::api::MessageBuffer::getInstance().get_message();
    }
}

static void filter_postprocess(MSFilter *f) {

}

static void filter_uninit(MSFilter *f) {
    auto *obj = (struct speechtext_enc_struct *) f->data;

    // Free resources
    ms_bufferizer_destroy(obj->bufferizer);
    ms_free(f->data);

    ms_message("Speechtext encoder - filter_uninit completed");
}


/*filter specific method*/

static int filter_set_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_enc_struct *) f->data;
    obj->sampleRate = *(int32_t *) arg;
    ms_message("Speechtext encoder - sample rate set to %d", obj->sampleRate);

    return 0;
}

static int filter_get_sample_rate(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_enc_struct *) f->data;
    *(int *) arg = obj->sampleRate;
    ms_message("Speechtext encoder - get_sample_rate called, sample rate is %i", obj->sampleRate);

    return 0;
}

#ifdef MS_AUDIO_ENCODER_SET_PTIME
static int filter_get_ptime(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_enc_struct *) f->data;
    *(int *) arg = obj->ptime;
    ms_message("Speechtext encoder - get_ptime called, ptime is %i", obj->ptime);

    return 0;
}

static int filter_set_ptime(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_enc_struct *) f->data;
    obj->ptime = *(int *) arg;
    if (obj->ptime % 20) {
        //if the ptime is not a multiple of 20, go to the next multiple
        obj->ptime = obj->ptime - obj->ptime % 20 + 20;
    }
    ms_message("Speechtext encoder - got ptime=%i", obj->ptime);

    return 0;
}
#endif

static int filter_add_fmtp(MSFilter *f, void *arg) {
    char buf[64] = {0};
    auto *obj = (struct speechtext_enc_struct *) f->data;
    const char *fmtp = (const char *) arg;

    if (fmtp_get_value(fmtp, "ptime", buf, sizeof(buf))) {
        int val = atoi(buf);
        filter_set_ptime(f, &val);
    } else
        ms_message("Speechtext encoder - unhandled fmtp %s", fmtp);

    return 0;
}

static int filter_get_nchannels(MSFilter *f, void *arg) {
    auto *obj = (struct speechtext_enc_struct *) f->data;
    *(int *) arg = obj->nchannels;
    return 0;
}

/*
 * plugin
 */
static MSFilterMethod filter_methods[] = {
        {MS_FILTER_SET_SAMPLE_RATE, filter_set_sample_rate},
        {MS_FILTER_GET_SAMPLE_RATE, filter_get_sample_rate},
        {MS_FILTER_GET_NCHANNELS, filter_get_nchannels},
        {MS_FILTER_ADD_FMTP, filter_add_fmtp},
#ifdef MS_AUDIO_ENCODER_SET_PTIME
        {MS_AUDIO_ENCODER_SET_PTIME, filter_set_ptime},
        {MS_AUDIO_ENCODER_GET_PTIME, filter_get_ptime},
#endif
        {0, NULL}
};

MSFilterDesc ms_speechtext_enc_desc = {
        .id=MS_FILTER_PLUGIN_ID, /* from Allfilters.h*/
        .name="MSSPEECHTEXTEnc",
        .text="Speechtext encoder filter",
        .category=MS_FILTER_ENCODER,
        .enc_fmt="SPEECHTEXT",
        .ninputs=1, /*number of inputs*/
        .noutputs=0, /*number of outputs*/
        .init=filter_init,
        .preprocess=NULL,
        .process=filter_process,
        .postprocess=filter_postprocess,
        .uninit=filter_uninit,
        .methods=filter_methods
};

MS_FILTER_DESC_EXPORT (ms_speechtext_enc_desc)

extern MSFilterDesc ms_speechtext_dec_desc;

#define MS_PLUGIN_DECLARE(type) type

MS_PLUGIN_DECLARE (void) libmsspeechtext_init(MSFactory *factory) {
    ms_factory_register_filter(factory, &ms_speechtext_enc_desc);
    ms_factory_register_filter(factory, &ms_speechtext_dec_desc);
    ms_message(" libmsspeechtext " VERSION " plugin loaded");
}
} // #extern "C"
