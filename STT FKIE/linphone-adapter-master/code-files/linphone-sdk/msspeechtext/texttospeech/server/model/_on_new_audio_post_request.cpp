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


#include "_on_new_audio_post_request.h"
#include "Helpers.h"

#include <sstream>

namespace org::openapitools::texttospeech::server::model
{

_on_new_audio_post_request::_on_new_audio_post_request()
{
    m_Length = 0;
    m_LengthIsSet = false;
    m_Sample_rate_hz = 0;
    m_Sample_rate_hzIsSet = false;
    m_Timestamp = 0L;
    m_TimestampIsSet = false;
    m_Message_id = "";
    m_Message_idIsSet = false;
    m_Byte_arrayIsSet = false;
    
}

void _on_new_audio_post_request::validate() const
{
    std::stringstream msg;
    if (!validate(msg))
    {
        throw org::openapitools::texttospeech::server::helpers::ValidationException(msg.str());
    }
}

bool _on_new_audio_post_request::validate(std::stringstream& msg) const
{
    return validate(msg, "");
}

bool _on_new_audio_post_request::validate(std::stringstream& msg, const std::string& pathPrefix) const
{
    bool success = true;
    const std::string _pathPrefix = pathPrefix.empty() ? "_on_new_audio_post_request" : pathPrefix;

                        
    return success;
}

bool _on_new_audio_post_request::operator==(const _on_new_audio_post_request& rhs) const
{
    return
    
    
    
    ((!lengthIsSet() && !rhs.lengthIsSet()) || (lengthIsSet() && rhs.lengthIsSet() && getLength() == rhs.getLength())) &&
    
    
    ((!sampleRateHzIsSet() && !rhs.sampleRateHzIsSet()) || (sampleRateHzIsSet() && rhs.sampleRateHzIsSet() && getSampleRateHz() == rhs.getSampleRateHz())) &&
    
    
    ((!timestampIsSet() && !rhs.timestampIsSet()) || (timestampIsSet() && rhs.timestampIsSet() && getTimestamp() == rhs.getTimestamp())) &&
    
    
    ((!messageIdIsSet() && !rhs.messageIdIsSet()) || (messageIdIsSet() && rhs.messageIdIsSet() && getMessageId() == rhs.getMessageId())) &&
    
    
    ((!byteArrayIsSet() && !rhs.byteArrayIsSet()) || (byteArrayIsSet() && rhs.byteArrayIsSet() && getByteArray() == rhs.getByteArray()))
    
    ;
}

bool _on_new_audio_post_request::operator!=(const _on_new_audio_post_request& rhs) const
{
    return !(*this == rhs);
}

void to_json(nlohmann::json& j, const _on_new_audio_post_request& o)
{
    j = nlohmann::json::object();
    if(o.lengthIsSet())
        j["length"] = o.m_Length;
    if(o.sampleRateHzIsSet())
        j["sample_rate_hz"] = o.m_Sample_rate_hz;
    if(o.timestampIsSet())
        j["timestamp"] = o.m_Timestamp;
    if(o.messageIdIsSet())
        j["message_id"] = o.m_Message_id;
    if(o.byteArrayIsSet())
        j["byte_array"] = o.m_Byte_array;
    
}

void from_json(const nlohmann::json& j, _on_new_audio_post_request& o)
{
    if(j.find("length") != j.end())
    {
        j.at("length").get_to(o.m_Length);
        o.m_LengthIsSet = true;
    } 
    if(j.find("sample_rate_hz") != j.end())
    {
        j.at("sample_rate_hz").get_to(o.m_Sample_rate_hz);
        o.m_Sample_rate_hzIsSet = true;
    } 
    if(j.find("timestamp") != j.end())
    {
        j.at("timestamp").get_to(o.m_Timestamp);
        o.m_TimestampIsSet = true;
    } 
    if(j.find("message_id") != j.end())
    {
        j.at("message_id").get_to(o.m_Message_id);
        o.m_Message_idIsSet = true;
    } 
    if(j.find("byte_array") != j.end())
    {
        j.at("byte_array").get_to(o.m_Byte_array);
        o.m_Byte_arrayIsSet = true;
    } 
    
}

int32_t _on_new_audio_post_request::getLength() const
{
    return m_Length;
}
void _on_new_audio_post_request::setLength(int32_t const value)
{
    m_Length = value;
    m_LengthIsSet = true;
}
bool _on_new_audio_post_request::lengthIsSet() const
{
    return m_LengthIsSet;
}
void _on_new_audio_post_request::unsetLength()
{
    m_LengthIsSet = false;
}
int32_t _on_new_audio_post_request::getSampleRateHz() const
{
    return m_Sample_rate_hz;
}
void _on_new_audio_post_request::setSampleRateHz(int32_t const value)
{
    m_Sample_rate_hz = value;
    m_Sample_rate_hzIsSet = true;
}
bool _on_new_audio_post_request::sampleRateHzIsSet() const
{
    return m_Sample_rate_hzIsSet;
}
void _on_new_audio_post_request::unsetSample_rate_hz()
{
    m_Sample_rate_hzIsSet = false;
}
int64_t _on_new_audio_post_request::getTimestamp() const
{
    return m_Timestamp;
}
void _on_new_audio_post_request::setTimestamp(int64_t const value)
{
    m_Timestamp = value;
    m_TimestampIsSet = true;
}
bool _on_new_audio_post_request::timestampIsSet() const
{
    return m_TimestampIsSet;
}
void _on_new_audio_post_request::unsetTimestamp()
{
    m_TimestampIsSet = false;
}
std::string _on_new_audio_post_request::getMessageId() const
{
    return m_Message_id;
}
void _on_new_audio_post_request::setMessageId(std::string const& value)
{
    m_Message_id = value;
    m_Message_idIsSet = true;
}
bool _on_new_audio_post_request::messageIdIsSet() const
{
    return m_Message_idIsSet;
}
void _on_new_audio_post_request::unsetMessage_id()
{
    m_Message_idIsSet = false;
}
std::string _on_new_audio_post_request::getByteArray() const
{
    return m_Byte_array;
}
void _on_new_audio_post_request::setByteArray(std::string const& value)
{
    m_Byte_array = value;
    m_Byte_arrayIsSet = true;
}
bool _on_new_audio_post_request::byteArrayIsSet() const
{
    return m_Byte_arrayIsSet;
}
void _on_new_audio_post_request::unsetByte_array()
{
    m_Byte_arrayIsSet = false;
}


} // namespace org::openapitools::texttospeech::server::model

