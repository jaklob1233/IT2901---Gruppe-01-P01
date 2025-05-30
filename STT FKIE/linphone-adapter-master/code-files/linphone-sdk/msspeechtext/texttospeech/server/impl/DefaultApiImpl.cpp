/**
* TextToSpeech
* This is an API for a Text-to-Speech component.
*
* The version of the OpenAPI document: 0.1.0
* Contact: norman.jansen@fkie.fraunhofer.de
*
* NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
* https://openapi-generator.tech
* Do not edit the class manually.
*/

#include "mediastreamer2/mscommon.h"
#include "DefaultApiImpl.h"
#include "../include/texttospeech/MessageBuffer.h"

namespace org::openapitools::texttospeech::server::api {

    using namespace org::openapitools::texttospeech::server::model;

    DefaultApiImpl::DefaultApiImpl(const std::shared_ptr<Pistache::Rest::Router> &rtr)
            : DefaultApi(rtr) {
    }

    void DefaultApiImpl::initialize(const std::optional<std::string> &texttospeechVariant,
                                    const std::optional<std::string> &configProfile,
                                    const std::optional<std::string> &webhookUrl,
                                    Pistache::Http::ResponseWriter &response) {
        ms_message("texttospeech-initialize - Do not use - is a dummy function");
        response.send(Pistache::Http::Code::Ok);
    }

    void DefaultApiImpl::on_new_audio_post(const _on_new_audio_post_request &onNewAudioPostRequest,
                                           Pistache::Http::ResponseWriter &response) {
        ms_message("texttospeech-on_new_audio_post, length=%d", onNewAudioPostRequest.getLength());

        // Calculate the length of the Base64 string corresponding to `totalBytesToDecode` decoded bytes
        size_t base64_length = (onNewAudioPostRequest.getLength() * 4 + 2) / 3; // Base64 characters required to represent `totalBytesToDecode` bytes

        // Get the actual Base64 string length (in characters)
        size_t actual_base64_length = onNewAudioPostRequest.getByteArray().size();

        // Check if the Base64 string length matches or exceeds the expected length
        if (actual_base64_length < base64_length) {
            std::ostringstream oss;
            oss << "Base64 length mismatch: Expected at least " << base64_length
                << " Base64 characters but found " << actual_base64_length;
            response.send(Pistache::Http::Code::Bad_Request, oss.str());
        }
        else {
            // put message in buffer
            org::openapitools::texttospeech::server::api::audio_post msg = {
                    onNewAudioPostRequest.getSampleRateHz(),
                    onNewAudioPostRequest.getLength(),
                    onNewAudioPostRequest.getTimestamp(),
                    onNewAudioPostRequest.getMessageId(),
                    onNewAudioPostRequest.getByteArray()
            };
            MessageBuffer::getInstance().add_message(msg);

            response.send(Pistache::Http::Code::Ok);
        }
    }

    void DefaultApiImpl::provide_text(const Provide_text_request &provideTextRequest,
                                      Pistache::Http::ResponseWriter &response) {
        ms_message("texttospeech-provide_text - Do not use - is a dummy function");
        response.send(Pistache::Http::Code::Ok);
    }
}

