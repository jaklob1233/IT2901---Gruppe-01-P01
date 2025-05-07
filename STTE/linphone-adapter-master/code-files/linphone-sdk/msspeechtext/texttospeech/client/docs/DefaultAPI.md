# DefaultAPI

All URIs are relative to *http://localhost:8082/texttospeech*

Method | HTTP request | Description
------------- | ------------- | -------------
[**Speechtotext_initialize**](DefaultAPI.md#Speechtotext_initialize) | **PUT** /initialize | Initialize the TextToSpeech component
[**Texttospeech_onNewAudioPost**](DefaultAPI.md#Texttospeech_onNewAudioPost) | **POST** /on_new_audio | 
[**Texttospeech_provideText**](DefaultAPI.md#Texttospeech_provideText) | **POST** /provide_text | Provide text to the TextToSpeech component


# **Speechtotext_initialize**
```c
// Initialize the TextToSpeech component
//
// Initialize the TextToSpeech component
//
void Speechtotext_initialize(apiClient_t *apiClient, texttospeech_initialize_texttospeech_variant_e texttospeech_variant, char *config_profile, char *webhook_url);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**texttospeech_variant** | **texttospeech_initialize_texttospeech_variant_e** |  | 
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

# **Texttospeech_onNewAudioPost**
```c
void Texttospeech_onNewAudioPost(apiClient_t *apiClient, _on_new_audio_post_request_t *_on_new_audio_post_request);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**_on_new_audio_post_request** | **[_on_new_audio_post_request_t](_on_new_audio_post_request.md) \*** | Webhook for geting part of the audio as byte array corresponding to the text provided | 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **Texttospeech_provideText**
```c
// Provide text to the TextToSpeech component
//
// Provide text to the TextToSpeech component
//
void Texttospeech_provideText(apiClient_t *apiClient, provide_text_request_t *provide_text_request);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**provide_text_request** | **[provide_text_request_t](provide_text_request.md) \*** | text for the TextToSpeech component | 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

