/**
* SpeechToText
* This is an API for a Speech-to-Text component.
*
* The version of the OpenAPI document: 0.1.0
* Contact: norman.jansen@fkie.fraunhofer.de
*
* NOTE: This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
* https://openapi-generator.tech
* Do not edit the class manually.
*/
/*
 * _on_new_text_post_request.h
 *
 * 
 */

#ifndef _on_new_text_post_request_H_
#define _on_new_text_post_request_H_


#include <string>
#include <nlohmann/json.hpp>

namespace org::openapitools::speechtotext::server::model
{

/// <summary>
/// 
/// </summary>
class  _on_new_text_post_request
{
public:
    _on_new_text_post_request();
    virtual ~_on_new_text_post_request() = default;


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

    bool operator==(const _on_new_text_post_request& rhs) const;
    bool operator!=(const _on_new_text_post_request& rhs) const;

    /////////////////////////////////////////////
    /// _on_new_text_post_request members

    /// <summary>
    /// timestamp (start) of the text corresponding to the audio samples
    /// </summary>
    int64_t getTimestampStart() const;
    void setTimestampStart(int32_t const value);
    bool timestampStartIsSet() const;
    void unsetTimestamp_start();
    /// <summary>
    /// timestamp (end) of the text corresponding to the audio samples
    /// </summary>
    int64_t getTimestampEnd() const;
    void setTimestampEnd(int32_t const value);
    bool timestampEndIsSet() const;
    void unsetTimestamp_end();
    /// <summary>
    /// 
    /// </summary>
    std::string getText() const;
    void setText(std::string const& value);
    bool textIsSet() const;
    void unsetText();

    friend  void to_json(nlohmann::json& j, const _on_new_text_post_request& o);
    friend  void from_json(const nlohmann::json& j, _on_new_text_post_request& o);
protected:
    int32_t m_Timestamp_start;
    bool m_Timestamp_startIsSet;
    int32_t m_Timestamp_end;
    bool m_Timestamp_endIsSet;
    std::string m_Text;
    bool m_TextIsSet;
    
};

} // namespace org::openapitools::speechtotext::server::model

#endif /* _on_new_text_post_request_H_ */
