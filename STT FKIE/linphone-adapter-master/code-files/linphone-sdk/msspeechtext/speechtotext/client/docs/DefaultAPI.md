# DefaultAPI

All URIs are relative to *http://localhost:8081/speechtotext*

Method | HTTP request | Description
------------- | ------------- | -------------
[**Speechtotext_initialize**](DefaultAPI.md#Speechtotext_initialize) | **PUT** /initialize | Initialize the SpeechToText component
[**Speechtotext_onNewTextPost**](DefaultAPI.md#Speechtotext_onNewTextPost) | **POST** /on_new_text | 
[**Speechtotext_provideAudio**](DefaultAPI.md#Speechtotext_provideAudio) | **POST** /provide_audio | Provide audio to the SpeechToText component


# **Speechtotext_initialize**
```c
// Initialize the SpeechToText component
//
// Initialize the SpeechToText component
//
void Speechtotext_initialize(apiClient_t *apiClient, speechtotext_initialize_speechtotext_variant_e speechtotext_variant, char *config_profile, char *webhook_url);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**speechtotext_variant** | **speechtotext_initialize_speechtotext_variant_e** |  | 
**config_profile** | **char \*** |  | 
**webhook_url** | **char \*** |  | 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **Speechtotext_onNewTextPost**
```c
void Speechtotext_onNewTextPost(apiClient_t *apiClient, _on_new_text_post_request_t *_on_new_text_post_request);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**_on_new_text_post_request** | **[_on_new_text_post_request_t](_on_new_text_post_request.md) \*** | Webhook for delivering new text from speech recognition to client | 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **Speechtotext_provideAudio**
```c
// Provide audio to the SpeechToText component
//
// Provide audio to the SpeechToText component
//
void Speechtotext_provideAudio(apiClient_t *apiClient, provide_audio_request_t *provide_audio_request);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**provide_audio_request** | **[provide_audio_request_t](provide_audio_request.md) \*** | part of the audio (e.g. 20 ms) as byte array | 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

