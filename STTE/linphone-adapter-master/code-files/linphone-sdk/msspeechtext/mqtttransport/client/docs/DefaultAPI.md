# DefaultAPI

All URIs are relative to *http://localhost:5000/mqtttransport*

Method | HTTP request | Description
------------- | ------------- | -------------
[**Speechtotext_initialize**](DefaultAPI.md#Speechtotext_initialize) | **PUT** /initialize | Initialize the MQTT-Transport component
[**DefaultAPI_onNewMessagePost**](DefaultAPI.md#DefaultAPI_onNewMessagePost) | **POST** /on_new_message | 
[**DefaultAPI_sendMessage**](DefaultAPI.md#DefaultAPI_sendMessage) | **POST** /send_message | Send message via MQTT Transport component


# **Speechtotext_initialize**
```c
// Initialize the MQTT-Transport component
//
// Initialize the MQTT-Transport component
//
void Speechtotext_initialize(apiClient_t *apiClient, mqtttransport_initialize_mqtt_variant_e mqtt_variant, char *config_profile, char *base_topic, char *webhook_url, char *mqtt_server_ip);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**mqtt_variant** | **mqtttransport_initialize_mqtt_variant_e** |  | 
**config_profile** | **char \*** |  | 
**base_topic** | **char \*** |  | 
**webhook_url** | **char \*** |  | 
**mqtt_server_ip** | **char \*** |  | [optional] 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: Not defined
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **DefaultAPI_onNewMessagePost**
```c
void DefaultAPI_onNewMessagePost(apiClient_t *apiClient, message_t *message);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**message** | **[message_t](message.md) \*** | Webhook for receiving message from MQTT Transport component | 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

# **DefaultAPI_sendMessage**
```c
// Send message via MQTT Transport component
//
// Send message via MQTT Transport component
//
void DefaultAPI_sendMessage(apiClient_t *apiClient, char *source, char *destination, message_t *message);
```

### Parameters
Name | Type | Description  | Notes
------------- | ------------- | ------------- | -------------
**apiClient** | **apiClient_t \*** | context containing the client configuration |
**source** | **char \*** | IP address of the source. | 
**destination** | **char \*** | IP address of the destination (optional, broadcast if not specified). | [optional] 
**message** | **[message_t](message.md) \*** | the message | [optional] 

### Return type

void

### Authorization

No authorization required

### HTTP request headers

 - **Content-Type**: application/json
 - **Accept**: application/json

[[Back to top]](#) [[Back to API list]](../README.md#documentation-for-api-endpoints) [[Back to Model list]](../README.md#documentation-for-models) [[Back to README]](../README.md)

