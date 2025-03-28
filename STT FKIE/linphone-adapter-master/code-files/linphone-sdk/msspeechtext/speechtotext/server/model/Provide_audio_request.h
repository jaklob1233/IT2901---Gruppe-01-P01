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
 * Provide_audio_request.h
 *
 * 
 */

#ifndef Provide_audio_request_H_
#define Provide_audio_request_H_


#include <string>
#include <nlohmann/json.hpp>

namespace org::openapitools::speechtotext::server::model
{

/// <summary>
/// 
/// </summary>
class  Provide_audio_request
{
public:
    Provide_audio_request();
    virtual ~Provide_audio_request() = default;


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

    bool operator==(const Provide_audio_request& rhs) const;
    bool operator!=(const Provide_audio_request& rhs) const;

    /////////////////////////////////////////////
    /// Provide_audio_request members

    /// <summary>
    /// length of the audio in bytes.
    /// </summary>
    int32_t getLength() const;
    void setLength(int32_t const value);
    bool lengthIsSet() const;
    void unsetLength();
    /// <summary>
    /// sample rate in hz.
    /// </summary>
    int32_t getSampleRateHz() const;
    void setSampleRateHz(int32_t const value);
    bool sampleRateHzIsSet() const;
    void unsetSample_rate_hz();
    /// <summary>
    /// timestamp of the start of the audio samples
    /// </summary>
    int64_t getTimestamp() const;
    void setTimestamp(int32_t const value);
    bool timestampIsSet() const;
    void unsetTimestamp();
    /// <summary>
    /// base64 encoded binary content.
    /// </summary>
    std::string getByteArray() const;
    void setByteArray(std::string const& value);
    bool byteArrayIsSet() const;
    void unsetByte_array();

    friend  void to_json(nlohmann::json& j, const Provide_audio_request& o);
    friend  void from_json(const nlohmann::json& j, Provide_audio_request& o);
protected:
    int32_t m_Length;
    bool m_LengthIsSet;
    int32_t m_Sample_rate_hz;
    bool m_Sample_rate_hzIsSet;
    int32_t m_Timestamp;
    bool m_TimestampIsSet;
    std::string m_Byte_array;
    bool m_Byte_arrayIsSet;
    
};

} // namespace org::openapitools::speechtotext::server::model

#endif /* Provide_audio_request_H_ */
