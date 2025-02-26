#include "mediastreamer2/mscommon.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "DefaultAPI.h"

#define MAX_NUMBER_LENGTH 16
#define MAX_BUFFER_LENGTH 4096
#define intToStr(dst, src) \
    do {\
    char dst[256];\
    snprintf(dst, 256, "%ld", (long int)(src));\
}while(0)

// Functions for enum MQTTVARIANT for DefaultAPI_initialize

static char* initialize_MQTTVARIANT_ToString(mqtttransport_initialize_mqtt_variant_e MQTTVARIANT){
    char *MQTTVARIANTArray[] =  { "NULL", "Standard", "MQTT-UDP" };
    return MQTTVARIANTArray[MQTTVARIANT];
}

static mqtttransport_initialize_mqtt_variant_e initialize_MQTTVARIANT_FromString(char* MQTTVARIANT){
    int stringToReturn = 0;
    char *MQTTVARIANTArray[] =  { "NULL", "Standard", "MQTT-UDP" };
    size_t sizeofArray = sizeof(MQTTVARIANTArray) / sizeof(MQTTVARIANTArray[0]);
    while(stringToReturn < sizeofArray) {
        if(strcmp(MQTTVARIANT, MQTTVARIANTArray[stringToReturn]) == 0) {
            return stringToReturn;
        }
        stringToReturn++;
    }
    return 0;
}

/*
// Function initialize_MQTTVARIANT_convertToJSON is not currently used,
// since conversion to JSON passes through the conversion of the model, and ToString. The function is kept for future reference.
//
static cJSON *initialize_MQTTVARIANT_convertToJSON(mqtttransport_initialize_mqtt_variant_e MQTTVARIANT) {
    cJSON *item = cJSON_CreateObject();
    if(cJSON_AddStringToObject(item, "mqtt_variant", initialize_MQTTVARIANT_ToString(MQTTVARIANT)) == NULL) {
        goto fail;
    }
    return item;
    fail:
    cJSON_Delete(item);
    return NULL;
}

// Function initialize_MQTTVARIANT_parseFromJSON is not currently used,
// since conversion from JSON passes through the conversion of the model, and FromString. The function is kept for future reference.
//
static mqtttransport_initialize_mqtt_variant_e initialize_MQTTVARIANT_parseFromJSON(cJSON* MQTTVARIANTJSON) {
    mqtttransport_initialize_mqtt_variant_e MQTTVARIANTVariable = 0;
    cJSON *MQTTVARIANTVar = cJSON_GetObjectItemCaseSensitive(MQTTVARIANTJSON, "mqtt_variant");
    if(!cJSON_IsString(MQTTVARIANTVar) || (MQTTVARIANTVar->valuestring == NULL))
    {
        goto end;
    }
    MQTTVARIANTVariable = initialize_MQTTVARIANT_FromString(MQTTVARIANTVar->valuestring);
    return MQTTVARIANTVariable;
end:
    return 0;
}
*/


// Initialize the MQTT-Transport component
//
// Initialize the MQTT-Transport component
//
void
Mqtttransport_initialize(mqtttransport_apiClient_t *apiClient, mqtttransport_initialize_mqtt_variant_e mqtt_variant, char *config_profile, char *base_topic, char *node_id, char *webhook_url, char *mqtt_server_ip, int *mqtt_server_port)
{
    list_t    *localVarQueryParameters = list_createList();
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = NULL;
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/initialize")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/initialize");




    // query parameters
    char *keyQuery_mqtt_variant = NULL;
    mqtttransport_initialize_mqtt_variant_e valueQuery_mqtt_variant ;
    keyValuePair_t *keyPairQuery_mqtt_variant = 0;
    if (mqtt_variant)
    {
        keyQuery_mqtt_variant = strdup("mqtt_variant");
        valueQuery_mqtt_variant = (mqtt_variant);
        keyPairQuery_mqtt_variant = keyValuePair_create(keyQuery_mqtt_variant, (void *)strdup(initialize_MQTTVARIANT_ToString(
        valueQuery_mqtt_variant)));
        list_addElement(localVarQueryParameters,keyPairQuery_mqtt_variant);
    }

    // query parameters
    char *keyQuery_config_profile = NULL;
    char * valueQuery_config_profile = NULL;
    keyValuePair_t *keyPairQuery_config_profile = 0;
    if (config_profile)
    {
        keyQuery_config_profile = strdup("config_profile");
        valueQuery_config_profile = strdup((config_profile));
        keyPairQuery_config_profile = keyValuePair_create(keyQuery_config_profile, valueQuery_config_profile);
        list_addElement(localVarQueryParameters,keyPairQuery_config_profile);
    }

    // query parameters
    char *keyQuery_base_topic = NULL;
    char * valueQuery_base_topic = NULL;
    keyValuePair_t *keyPairQuery_base_topic = 0;
    if (base_topic)
    {
        keyQuery_base_topic = strdup("base_topic");
        valueQuery_base_topic = strdup((base_topic));
        keyPairQuery_base_topic = keyValuePair_create(keyQuery_base_topic, valueQuery_base_topic);
        list_addElement(localVarQueryParameters,keyPairQuery_base_topic);
    }

    // query parameters
    char *keyQuery_mqtt_server_ip = NULL;
    char * valueQuery_mqtt_server_ip = NULL;
    keyValuePair_t *keyPairQuery_mqtt_server_ip = 0;
    if (mqtt_server_ip)
    {
        keyQuery_mqtt_server_ip = strdup("mqtt_server_ip");
        valueQuery_mqtt_server_ip = strdup((mqtt_server_ip));
        keyPairQuery_mqtt_server_ip = keyValuePair_create(keyQuery_mqtt_server_ip, valueQuery_mqtt_server_ip);
        list_addElement(localVarQueryParameters,keyPairQuery_mqtt_server_ip);
    }

    // query parameters
    char *keyQuery_mqtt_server_port = NULL;
    char * valueQuery_mqtt_server_port = NULL;
    keyValuePair_t *keyPairQuery_mqtt_server_port = 0;
    if (mqtt_server_port)
    {
        keyQuery_mqtt_server_port = strdup("mqtt_server_port");
        valueQuery_mqtt_server_port = calloc(1,MAX_NUMBER_LENGTH);
        snprintf(valueQuery_mqtt_server_port, MAX_NUMBER_LENGTH, "%d", *mqtt_server_port);
        keyPairQuery_mqtt_server_port = keyValuePair_create(keyQuery_mqtt_server_port, valueQuery_mqtt_server_port);
        list_addElement(localVarQueryParameters,keyPairQuery_mqtt_server_port);
    }

    // query parameters
    char *keyQuery_node_id = NULL;
    char * valueQuery_node_id = NULL;
    keyValuePair_t *keyPairQuery_node_id = 0;
    if (node_id)
    {
        keyQuery_node_id = strdup("node_id");
        valueQuery_node_id = strdup((node_id));
        keyPairQuery_node_id = keyValuePair_create(keyQuery_node_id, valueQuery_node_id);
        list_addElement(localVarQueryParameters,keyPairQuery_node_id);
    }

    // query parameters
    char *keyQuery_webhook_url = NULL;
    char * valueQuery_webhook_url = NULL;
    keyValuePair_t *keyPairQuery_webhook_url = 0;
    if (webhook_url)
    {
        keyQuery_webhook_url = strdup("webhook_url");
        valueQuery_webhook_url = strdup((webhook_url));
        keyPairQuery_webhook_url = keyValuePair_create(keyQuery_webhook_url, valueQuery_webhook_url);
        list_addElement(localVarQueryParameters,keyPairQuery_webhook_url);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    mqtttransport_apiClient_invoke(apiClient,
                    localVarPath,
                    localVarQueryParameters,
                    localVarHeaderParameters,
                    localVarFormParameters,
                    localVarHeaderType,
                    localVarContentType,
                    localVarBodyParameters,
                    "PUT");

    // uncomment below to debug the error response
    //if (apiClient->response_code == 200) {
    //    printf("%s\n","Success");
    //}
    // uncomment below to debug the error response
    //if (apiClient->response_code == 400) {
    //    printf("%s\n","Error");
    //}
    //No return type
end:
    if (apiClient->dataReceived) {
        free(apiClient->dataReceived);
        apiClient->dataReceived = NULL;
        apiClient->dataReceivedLen = 0;
    }
    list_freeList(localVarQueryParameters);
    
    
    list_freeList(localVarHeaderType);
    
    free(localVarPath);
    if(keyQuery_mqtt_variant){
        free(keyQuery_mqtt_variant);
        keyQuery_mqtt_variant = NULL;
    }
    if(valueQuery_mqtt_variant){
//        free(valueQuery_mqtt_variant);
        valueQuery_mqtt_variant = mqtttransport_initialize_MQTTVARIANT_NULL;
    }
    if(keyPairQuery_mqtt_variant){
        keyValuePair_free(keyPairQuery_mqtt_variant);
        keyPairQuery_mqtt_variant = NULL;
    }
    if(keyQuery_mqtt_variant){
        free(keyQuery_mqtt_variant);
        keyQuery_mqtt_variant = NULL;
    }
    if(keyPairQuery_mqtt_variant){
        keyValuePair_free(keyPairQuery_mqtt_variant);
        keyPairQuery_mqtt_variant = NULL;
    }
    if(keyQuery_config_profile){
        free(keyQuery_config_profile);
        keyQuery_config_profile = NULL;
    }
    if(valueQuery_config_profile){
        free(valueQuery_config_profile);
        valueQuery_config_profile = NULL;
    }
    if(keyPairQuery_config_profile){
        keyValuePair_free(keyPairQuery_config_profile);
        keyPairQuery_config_profile = NULL;
    }
    if(keyQuery_config_profile){
        free(keyQuery_config_profile);
        keyQuery_config_profile = NULL;
    }
    if(keyPairQuery_config_profile){
        keyValuePair_free(keyPairQuery_config_profile);
        keyPairQuery_config_profile = NULL;
    }
    if(keyQuery_base_topic){
        free(keyQuery_base_topic);
        keyQuery_base_topic = NULL;
    }
    if(valueQuery_base_topic){
        free(valueQuery_base_topic);
        valueQuery_base_topic = NULL;
    }
    if(keyPairQuery_base_topic){
        keyValuePair_free(keyPairQuery_base_topic);
        keyPairQuery_base_topic = NULL;
    }
    if(keyQuery_base_topic){
        free(keyQuery_base_topic);
        keyQuery_base_topic = NULL;
    }
    if(keyPairQuery_base_topic){
        keyValuePair_free(keyPairQuery_base_topic);
        keyPairQuery_base_topic = NULL;
    }
    if(keyQuery_mqtt_server_ip){
        free(keyQuery_mqtt_server_ip);
        keyQuery_mqtt_server_ip = NULL;
    }
    if(valueQuery_mqtt_server_ip){
        free(valueQuery_mqtt_server_ip);
        valueQuery_mqtt_server_ip = NULL;
    }
    if(keyPairQuery_mqtt_server_ip){
        keyValuePair_free(keyPairQuery_mqtt_server_ip);
        keyPairQuery_mqtt_server_ip = NULL;
    }
    if(keyQuery_mqtt_server_ip){
        free(keyQuery_mqtt_server_ip);
        keyQuery_mqtt_server_ip = NULL;
    }
    if(keyPairQuery_mqtt_server_ip){
        keyValuePair_free(keyPairQuery_mqtt_server_ip);
        keyPairQuery_mqtt_server_ip = NULL;
    }
    if(keyQuery_node_id){
        free(keyQuery_node_id);
        keyQuery_node_id = NULL;
    }
    if(valueQuery_node_id){
        free(valueQuery_node_id);
        valueQuery_node_id = NULL;
    }
    if(keyPairQuery_node_id){
        keyValuePair_free(keyPairQuery_node_id);
        keyPairQuery_node_id = NULL;
    }
    if(keyQuery_node_id){
        free(keyQuery_node_id);
        keyQuery_node_id = NULL;
    }
    if(keyPairQuery_node_id){
        keyValuePair_free(keyPairQuery_node_id);
        keyPairQuery_node_id = NULL;
    }
    if(keyQuery_webhook_url){
        free(keyQuery_webhook_url);
        keyQuery_webhook_url = NULL;
    }
    if(valueQuery_webhook_url){
        free(valueQuery_webhook_url);
        valueQuery_webhook_url = NULL;
    }
    if(keyPairQuery_webhook_url){
        keyValuePair_free(keyPairQuery_webhook_url);
        keyPairQuery_webhook_url = NULL;
    }
    if(keyQuery_webhook_url){
        free(keyQuery_webhook_url);
        keyQuery_webhook_url = NULL;
    }
    if(keyPairQuery_webhook_url){
        keyValuePair_free(keyPairQuery_webhook_url);
        keyPairQuery_webhook_url = NULL;
    }

}

void
DMqtttransport_onNewMessagePost(mqtttransport_apiClient_t *apiClient, message_t *message)
{
    list_t    *localVarQueryParameters = NULL;
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = list_createList();
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/on_new_message")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/on_new_message");




    // Body Param
    cJSON *localVarSingleItemJSON_message = NULL;
    if (message != NULL)
    {
        //string
        localVarSingleItemJSON_message = message_convertToJSON(message);
        localVarBodyParameters = cJSON_Print(localVarSingleItemJSON_message);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    list_addElement(localVarContentType,"application/json"); //consumes
    mqtttransport_apiClient_invoke(apiClient,
                    localVarPath,
                    localVarQueryParameters,
                    localVarHeaderParameters,
                    localVarFormParameters,
                    localVarHeaderType,
                    localVarContentType,
                    localVarBodyParameters,
                    "POST");

    // uncomment below to debug the error response
//    if (apiClient->response_code == 200) {
//        ms_message("Norman: Success");
//        printf("%s\n","Success");
//    }
    // uncomment below to debug the error response
//    if (apiClient->response_code == 400) {
//        ms_message("Norman: Error");
//        printf("%s\n","Error");
//    }
    //No return type
end:
    if (apiClient->dataReceived) {
        free(apiClient->dataReceived);
        apiClient->dataReceived = NULL;
        apiClient->dataReceivedLen = 0;
    }
    
    
    
    list_freeList(localVarHeaderType);
    list_freeList(localVarContentType);
    free(localVarPath);
    if (localVarSingleItemJSON_message) {
        cJSON_Delete(localVarSingleItemJSON_message);
        localVarSingleItemJSON_message = NULL;
    }
    free(localVarBodyParameters);

}

// Send message via MQTT Transport component
//
// Send message via MQTT Transport component
//
void
Mqtttransport_sendMessage(mqtttransport_apiClient_t *apiClient, char *source, char *destination, message_t *message)
{
    list_t    *localVarQueryParameters = list_createList();
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = list_createList();
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/send_message")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/send_message");




    // query parameters
    char *keyQuery_source = NULL;
    char * valueQuery_source = NULL;
    keyValuePair_t *keyPairQuery_source = 0;
    if (source)
    {
        keyQuery_source = strdup("source");
        valueQuery_source = strdup((source));
        keyPairQuery_source = keyValuePair_create(keyQuery_source, valueQuery_source);
        list_addElement(localVarQueryParameters,keyPairQuery_source);
    }

    // query parameters
    char *keyQuery_destination = NULL;
    char * valueQuery_destination = NULL;
    keyValuePair_t *keyPairQuery_destination = 0;
    if (destination)
    {
        keyQuery_destination = strdup("destination");
        valueQuery_destination = strdup((destination));
        keyPairQuery_destination = keyValuePair_create(keyQuery_destination, valueQuery_destination);
        list_addElement(localVarQueryParameters,keyPairQuery_destination);
    }

    // Body Param
    cJSON *localVarSingleItemJSON_message = NULL;
    if (message != NULL)
    {
        //string
        localVarSingleItemJSON_message = message_convertToJSON(message);
        localVarBodyParameters = cJSON_Print(localVarSingleItemJSON_message);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    list_addElement(localVarContentType,"application/json"); //consumes
    mqtttransport_apiClient_invoke(apiClient,
                    localVarPath,
                    localVarQueryParameters,
                    localVarHeaderParameters,
                    localVarFormParameters,
                    localVarHeaderType,
                    localVarContentType,
                    localVarBodyParameters,
                    "POST");

    // uncomment below to debug the error response
//    if (apiClient->response_code == 200) {
//        ms_message("%s\n","Success");
//        printf("%s\n","Success");
//    }
    // uncomment below to debug the error response
//    if (apiClient->response_code == 400) {
//        ms_message("%s\n","Error");
//        printf("%s\n","Error");
//    }
    //No return type
end:
    if (apiClient->dataReceived) {
        free(apiClient->dataReceived);
        apiClient->dataReceived = NULL;
        apiClient->dataReceivedLen = 0;
    }
    list_freeList(localVarQueryParameters);
    
    
    list_freeList(localVarHeaderType);
    list_freeList(localVarContentType);
    free(localVarPath);
    if (localVarSingleItemJSON_message) {
        cJSON_Delete(localVarSingleItemJSON_message);
        localVarSingleItemJSON_message = NULL;
    }
    free(localVarBodyParameters);
    if(keyQuery_source){
        free(keyQuery_source);
        keyQuery_source = NULL;
    }
    if(valueQuery_source){
        free(valueQuery_source);
        valueQuery_source = NULL;
    }
    if(keyPairQuery_source){
        keyValuePair_free(keyPairQuery_source);
        keyPairQuery_source = NULL;
    }
    if(keyQuery_source){
        free(keyQuery_source);
        keyQuery_source = NULL;
    }
    if(keyPairQuery_source){
        keyValuePair_free(keyPairQuery_source);
        keyPairQuery_source = NULL;
    }
    if(keyQuery_destination){
        free(keyQuery_destination);
        keyQuery_destination = NULL;
    }
    if(valueQuery_destination){
        free(valueQuery_destination);
        valueQuery_destination = NULL;
    }
    if(keyPairQuery_destination){
        keyValuePair_free(keyPairQuery_destination);
        keyPairQuery_destination = NULL;
    }
    if(keyQuery_destination){
        free(keyQuery_destination);
        keyQuery_destination = NULL;
    }
    if(keyPairQuery_destination){
        keyValuePair_free(keyPairQuery_destination);
        keyPairQuery_destination = NULL;
    }

}

