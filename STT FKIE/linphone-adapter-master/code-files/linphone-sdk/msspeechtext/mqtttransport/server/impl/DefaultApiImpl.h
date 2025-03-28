/**
* MqttTransport
* This is an API for a MQTT-Transport component.
*
* The version of the OpenAPI document: 0.1.0
* Contact: norman.jansen@fkie.fraunhofer.de
*
* NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
* https://openapi-generator.tech
* Do not edit the class manually.
*/

/*
* DefaultApiImpl.h
*
* 
*/

#ifndef DEFAULT_MQTTTRANSPORT_API_IMPL_H_
#define DEFAULT_MQTTTRANSPORT_API_IMPL_H_


#include <pistache/endpoint.h>
#include <pistache/http.h>
#include <pistache/router.h>
#include <memory>
#include <optional>

#include "../api/DefaultApi.h"


#include "../model/Error.h"
#include "../model/Message.h"
#include <string>

namespace org::openapitools::mqtttransport::server::api
{

using namespace org::openapitools::mqtttransport::server::model;

class  DefaultApiImpl : public org::openapitools::mqtttransport::server::api::DefaultApi {
public:
    explicit DefaultApiImpl(const std::shared_ptr<Pistache::Rest::Router>& rtr);
    ~DefaultApiImpl() override = default;

    void initialize(const std::optional<std::string> &mqttVariant, const std::optional<std::string> &configProfile, const std::optional<std::string> &baseTopic, const std::optional<std::string> &nodeId, const std::optional<std::string> &webhookUrl, const std::optional<std::string> &mqttServerIp, const std::optional<int32_t> &mqttServerPort, Pistache::Http::ResponseWriter &response);
    void on_new_message_post(const Message &message, Pistache::Http::ResponseWriter &response);
    void send_message(const std::optional<std::string> &source, const std::optional<std::string> &destination, const Message &message, Pistache::Http::ResponseWriter &response);

};

} // namespace org::openapitools::server::api



#endif
