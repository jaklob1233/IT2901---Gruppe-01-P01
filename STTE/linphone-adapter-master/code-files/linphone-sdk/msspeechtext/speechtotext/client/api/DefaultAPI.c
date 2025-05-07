#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "DefaultAPI.h"
#include "mediastreamer2/mscommon.h"

#define MAX_NUMBER_LENGTH 16
#define MAX_BUFFER_LENGTH 4096
#define intToStr(dst, src) \
    do {\
    char dst[256];\
    snprintf(dst, 256, "%ld", (long int)(src));\
}while(0)

// Functions for enum SPEECHTOTEXTVARIANT for DefaultAPI_initialize

static char* initialize_SPEECHTOTEXTVARIANT_ToString(speechtotext_initialize_speechtotext_variant_e SPEECHTOTEXTVARIANT){
    char *SPEECHTOTEXTVARIANTArray[] =  { "NULL", "Whisper", "Vosk" };
    return SPEECHTOTEXTVARIANTArray[SPEECHTOTEXTVARIANT];
}

static speechtotext_initialize_speechtotext_variant_e initialize_SPEECHTOTEXTVARIANT_FromString(char* SPEECHTOTEXTVARIANT){
    int stringToReturn = 0;
    char *SPEECHTOTEXTVARIANTArray[] =  { "NULL", "Whisper", "Vosk" };
    size_t sizeofArray = sizeof(SPEECHTOTEXTVARIANTArray) / sizeof(SPEECHTOTEXTVARIANTArray[0]);
    while(stringToReturn < sizeofArray) {
        if(strcmp(SPEECHTOTEXTVARIANT, SPEECHTOTEXTVARIANTArray[stringToReturn]) == 0) {
            return stringToReturn;
        }
        stringToReturn++;
    }
    return 0;
}

/*
// Function initialize_SPEECHTOTEXTVARIANT_convertToJSON is not currently used,
// since conversion to JSON passes through the conversion of the model, and ToString. The function is kept for future reference.
//
static cJSON *initialize_SPEECHTOTEXTVARIANT_convertToJSON(speechtotext_initialize_speechtotext_variant_e SPEECHTOTEXTVARIANT) {
    cJSON *item = cJSON_CreateObject();
    if(cJSON_AddStringToObject(item, "speechtotext_variant", initialize_SPEECHTOTEXTVARIANT_ToString(SPEECHTOTEXTVARIANT)) == NULL) {
        goto fail;
    }
    return item;
    fail:
    cJSON_Delete(item);
    return NULL;
}

// Function initialize_SPEECHTOTEXTVARIANT_parseFromJSON is not currently used,
// since conversion from JSON passes through the conversion of the model, and FromString. The function is kept for future reference.
//
static speechtotext_initialize_speechtotext_variant_e initialize_SPEECHTOTEXTVARIANT_parseFromJSON(cJSON* SPEECHTOTEXTVARIANTJSON) {
    speechtotext_initialize_speechtotext_variant_e SPEECHTOTEXTVARIANTVariable = 0;
    cJSON *SPEECHTOTEXTVARIANTVar = cJSON_GetObjectItemCaseSensitive(SPEECHTOTEXTVARIANTJSON, "speechtotext_variant");
    if(!cJSON_IsString(SPEECHTOTEXTVARIANTVar) || (SPEECHTOTEXTVARIANTVar->valuestring == NULL))
    {
        goto end;
    }
    SPEECHTOTEXTVARIANTVariable = initialize_SPEECHTOTEXTVARIANT_FromString(SPEECHTOTEXTVARIANTVar->valuestring);
    return SPEECHTOTEXTVARIANTVariable;
end:
    return 0;
}
*/


// Initialize the SpeechToText component
//
// Initialize the SpeechToText component
//
void
Speechtotext_initialize(speechtotext_apiClient_t *apiClient, speechtotext_initialize_speechtotext_variant_e speechtotext_variant, char *config_profile, char *webhook_url)
{
    list_t    *localVarQueryParameters = list_createList();
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = NULL;
    char      *localVarBodyParameters = NULL;

    // create the path
    ulong sizeOfPath = strlen("/initialize")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/initialize");




    // query parameters
    char *keyQuery_speechtotext_variant = NULL;
    speechtotext_initialize_speechtotext_variant_e valueQuery_speechtotext_variant ;
    keyValuePair_t *keyPairQuery_speechtotext_variant = 0;
    if (speechtotext_variant)
    {
        keyQuery_speechtotext_variant = strdup("speechtotext_variant");
        valueQuery_speechtotext_variant = (speechtotext_variant);
        keyPairQuery_speechtotext_variant = keyValuePair_create(keyQuery_speechtotext_variant, (void *)strdup(initialize_SPEECHTOTEXTVARIANT_ToString(
        valueQuery_speechtotext_variant)));
        list_addElement(localVarQueryParameters,keyPairQuery_speechtotext_variant);
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
    speechtotext_apiClient_invoke(apiClient,
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
    if(keyQuery_speechtotext_variant){
        free(keyQuery_speechtotext_variant);
        keyQuery_speechtotext_variant = NULL;
    }
    if(valueQuery_speechtotext_variant){
//        free(valueQuery_speechtotext_variant);
        valueQuery_speechtotext_variant = speechtotext_initialize_SPEECHTOTEXTVARIANT_NULL;
    }
    if(keyPairQuery_speechtotext_variant){
        keyValuePair_free(keyPairQuery_speechtotext_variant);
        keyPairQuery_speechtotext_variant = NULL;
    }
    if(keyQuery_speechtotext_variant){
        free(keyQuery_speechtotext_variant);
        keyQuery_speechtotext_variant = NULL;
    }
    if(keyPairQuery_speechtotext_variant){
        keyValuePair_free(keyPairQuery_speechtotext_variant);
        keyPairQuery_speechtotext_variant = NULL;
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
Speechtotext_onNewTextPost(speechtotext_apiClient_t *apiClient, _on_new_text_post_request_t *_on_new_text_post_request)
{
    list_t    *localVarQueryParameters = NULL;
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = list_createList();
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/on_new_text")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/on_new_text");




    // Body Param
    cJSON *localVarSingleItemJSON__on_new_text_post_request = NULL;
    if (_on_new_text_post_request != NULL)
    {
        //string
        localVarSingleItemJSON__on_new_text_post_request = _on_new_text_post_request_convertToJSON(_on_new_text_post_request);
        localVarBodyParameters = cJSON_Print(localVarSingleItemJSON__on_new_text_post_request);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    list_addElement(localVarContentType,"application/json"); //consumes
    speechtotext_apiClient_invoke(apiClient,
                    localVarPath,
                    localVarQueryParameters,
                    localVarHeaderParameters,
                    localVarFormParameters,
                    localVarHeaderType,
                    localVarContentType,
                    localVarBodyParameters,
                    "POST");

    // uncomment below to debug the error response
    //if (apiClient->response_code == 200) {
    //    printf("%s\n","Webhook acknowledged");
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



    list_freeList(localVarHeaderType);
    list_freeList(localVarContentType);
    free(localVarPath);
    if (localVarSingleItemJSON__on_new_text_post_request) {
        cJSON_Delete(localVarSingleItemJSON__on_new_text_post_request);
        localVarSingleItemJSON__on_new_text_post_request = NULL;
    }
    free(localVarBodyParameters);

}

// Provide audio to the SpeechToText component
//
// Provide audio to the SpeechToText component
//
void
Speechtotext_provideAudio(speechtotext_apiClient_t *apiClient, provide_audio_request_t *provide_audio_request)
{
    list_t    *localVarQueryParameters = NULL;
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = list_createList();
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/provide_audio")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/provide_audio");


    // Body Param
    cJSON *localVarSingleItemJSON_provide_audio_request = NULL;
    if (provide_audio_request != NULL)
    {
        //string
        localVarSingleItemJSON_provide_audio_request = provide_audio_request_convertToJSON(provide_audio_request);
        localVarBodyParameters = cJSON_Print(localVarSingleItemJSON_provide_audio_request);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    list_addElement(localVarContentType,"application/json"); //consumes
    speechtotext_apiClient_invoke(apiClient,
                    localVarPath,
                    localVarQueryParameters,
                    localVarHeaderParameters,
                    localVarFormParameters,
                    localVarHeaderType,
                    localVarContentType,
                    localVarBodyParameters,
                    "POST");

    // uncomment below to debug the error response
     if (apiClient->response_code == 200) {
     ms_message("Norman-Client: speechtotext-provide_audio - %s","Success");
     }
    // uncomment below to debug the error response
     if (apiClient->response_code == 400) {
     ms_message("Norman-Client: speechtotext-provide_audio - %s","Error");
     }
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
    if (localVarSingleItemJSON_provide_audio_request) {
        cJSON_Delete(localVarSingleItemJSON_provide_audio_request);
        localVarSingleItemJSON_provide_audio_request = NULL;
    }
    free(localVarBodyParameters);

}

