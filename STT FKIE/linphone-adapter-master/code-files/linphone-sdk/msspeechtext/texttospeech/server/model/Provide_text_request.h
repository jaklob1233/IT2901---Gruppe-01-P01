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
/*
 * Provide_text_request.h
 *
 * 
 */

#ifndef Provide_text_request_H_
#define Provide_text_request_H_


#include <string>
#include <nlohmann/json.hpp>

namespace org::openapitools::texttospeech::server::model
{

/// <summary>
/// 
/// </summary>
class  Provide_text_request
{
public:
    Provide_text_request();
    virtual ~Provide_text_request() = default;


    /// <summary>
    /// Validate the current data in the model. Throws a ValidationException on failure.
    /// </summary>
    void validate() const;

    /// <summary>
    /// Validate the current data in the model. Returns false on error and writes an error
    /// message into the given stringstream.
    /// </summary>
    bool validate(std::stringstream& msg) const;

    /// <summary>
    /// Helper overload for validate. Used when one model stores another model and calls it's validate.
    /// Not meant to be called outside that case.
    /// </summary>
    bool validate(std::stringstream& msg, const std::string& pathPrefix) const;

    bool operator==(const Provide_text_request& rhs) const;
    bool operator!=(const Provide_text_request& rhs) const;

    /////////////////////////////////////////////
    /// Provide_text_request members

    /// <summary>
    /// timestamp (start) of the text corresponding to the audio samples in ms
    /// </summary>
    int64_t getTimestampStart() const;
    void setTimestampStart(int64_t const value);
    bool timestampStartIsSet() const;
    void unsetTimestamp_start();
    /// <summary>
    /// timestamp (end) of the text corresponding to the audio samples in ms
    /// </summary>
    int64_t getTimestampEnd() const;
    void setTimestampEnd(int64_t const value);
    bool timestampEndIsSet() const;
    void unsetTimestamp_end();
    /// <summary>
    /// 
    /// </summary>
    std::string getMessageId() const;
    void setMessageId(std::string const& value);
    bool messageIdIsSet() const;
    void unsetMessage_id();
    /// <summary>
    /// 
    /// </summary>
    std::string getText() const;
    void setText(std::string const& value);
    bool textIsSet() const;
    void unsetText();

    friend  void to_json(nlohmann::json& j, const Provide_text_request& o);
    friend  void from_json(const nlohmann::json& j, Provide_text_request& o);
protected:
    int64_t m_Timestamp_start;
    bool m_Timestamp_startIsSet;
    int64_t m_Timestamp_end;
    bool m_Timestamp_endIsSet;
    std::string m_Message_id;
    bool m_Message_idIsSet;
    std::string m_Text;
    bool m_TextIsSet;
    
};

} // namespace org::openapitools::texttospeech::server::model

#endif /* Provide_text_request_H_ */
