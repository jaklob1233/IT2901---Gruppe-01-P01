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

// Functions for enum TEXTTOSPEECHVARIANT for DefaultAPI_initialize

static char* initialize_TEXTTOSPEECHVARIANT_ToString(texttospeech_initialize_texttospeech_variant_e TEXTTOSPEECHVARIANT){
    char *TEXTTOSPEECHVARIANTArray[] =  { "NULL", "standard", "coqui_yourtts", "coqui_xtts" };
    return TEXTTOSPEECHVARIANTArray[TEXTTOSPEECHVARIANT];
}

static texttospeech_initialize_texttospeech_variant_e initialize_TEXTTOSPEECHVARIANT_FromString(char* TEXTTOSPEECHVARIANT){
    int stringToReturn = 0;
    char *TEXTTOSPEECHVARIANTArray[] =  { "NULL", "standard", "coqui_yourtts", "coqui_xtts" };
    size_t sizeofArray = sizeof(TEXTTOSPEECHVARIANTArray) / sizeof(TEXTTOSPEECHVARIANTArray[0]);
    while(stringToReturn < sizeofArray) {
        if(strcmp(TEXTTOSPEECHVARIANT, TEXTTOSPEECHVARIANTArray[stringToReturn]) == 0) {
            return stringToReturn;
        }
        stringToReturn++;
    }
    return 0;
}

/*
// Function initialize_TEXTTOSPEECHVARIANT_convertToJSON is not currently used,
// since conversion to JSON passes through the conversion of the model, and ToString. The function is kept for future reference.
//
static cJSON *initialize_TEXTTOSPEECHVARIANT_convertToJSON(texttospeech_initialize_texttospeech_variant_e TEXTTOSPEECHVARIANT) {
    cJSON *item = cJSON_CreateObject();
    if(cJSON_AddStringToObject(item, "texttospeech_variant", initialize_TEXTTOSPEECHVARIANT_ToString(TEXTTOSPEECHVARIANT)) == NULL) {
        goto fail;
    }
    return item;
    fail:
    cJSON_Delete(item);
    return NULL;
}

// Function initialize_TEXTTOSPEECHVARIANT_parseFromJSON is not currently used,
// since conversion from JSON passes through the conversion of the model, and FromString. The function is kept for future reference.
//
static texttospeech_initialize_texttospeech_variant_e initialize_TEXTTOSPEECHVARIANT_parseFromJSON(cJSON* TEXTTOSPEECHVARIANTJSON) {
    texttospeech_initialize_texttospeech_variant_e TEXTTOSPEECHVARIANTVariable = 0;
    cJSON *TEXTTOSPEECHVARIANTVar = cJSON_GetObjectItemCaseSensitive(TEXTTOSPEECHVARIANTJSON, "texttospeech_variant");
    if(!cJSON_IsString(TEXTTOSPEECHVARIANTVar) || (TEXTTOSPEECHVARIANTVar->valuestring == NULL))
    {
        goto end;
    }
    TEXTTOSPEECHVARIANTVariable = initialize_TEXTTOSPEECHVARIANT_FromString(TEXTTOSPEECHVARIANTVar->valuestring);
    return TEXTTOSPEECHVARIANTVariable;
end:
    return 0;
}
*/


// Initialize the TextToSpeech component
//
// Initialize the TextToSpeech component
//
void
Texttospeech_initialize(texttospeech_apiClient_t *apiClient, texttospeech_initialize_texttospeech_variant_e texttospeech_variant, char *config_profile, char *webhook_url)
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
    char *keyQuery_texttospeech_variant = NULL;
    texttospeech_initialize_texttospeech_variant_e valueQuery_texttospeech_variant ;
    keyValuePair_t *keyPairQuery_texttospeech_variant = 0;
    if (texttospeech_variant)
    {
        keyQuery_texttospeech_variant = strdup("texttospeech_variant");
        valueQuery_texttospeech_variant = (texttospeech_variant);
        keyPairQuery_texttospeech_variant = keyValuePair_create(keyQuery_texttospeech_variant, (void *)strdup(initialize_TEXTTOSPEECHVARIANT_ToString(
        valueQuery_texttospeech_variant)));
        list_addElement(localVarQueryParameters,keyPairQuery_texttospeech_variant);
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
    texttospeech_apiClient_invoke(apiClient,
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
    if(keyQuery_texttospeech_variant){
        free(keyQuery_texttospeech_variant);
        keyQuery_texttospeech_variant = NULL;
    }
    if(valueQuery_texttospeech_variant){
//        free(valueQuery_texttospeech_variant);
        valueQuery_texttospeech_variant = texttospeech_initialize_TEXTTOSPEECHVARIANT_NULL;
    }
    if(keyPairQuery_texttospeech_variant){
        keyValuePair_free(keyPairQuery_texttospeech_variant);
        keyPairQuery_texttospeech_variant = NULL;
    }
    if(keyQuery_texttospeech_variant){
        free(keyQuery_texttospeech_variant);
        keyQuery_texttospeech_variant = NULL;
    }
    if(keyPairQuery_texttospeech_variant){
        keyValuePair_free(keyPairQuery_texttospeech_variant);
        keyPairQuery_texttospeech_variant = NULL;
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
Texttospeech_onNewAudioPost(texttospeech_apiClient_t *apiClient, _on_new_audio_post_request_t *_on_new_audio_post_request)
{
    list_t    *localVarQueryParameters = NULL;
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = list_createList();
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/on_new_audio")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/on_new_audio");




    // Body Param
    cJSON *localVarSingleItemJSON__on_new_audio_post_request = NULL;
    if (_on_new_audio_post_request != NULL)
    {
        //string
        localVarSingleItemJSON__on_new_audio_post_request = _on_new_audio_post_request_convertToJSON(_on_new_audio_post_request);
        localVarBodyParameters = cJSON_Print(localVarSingleItemJSON__on_new_audio_post_request);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    list_addElement(localVarContentType,"application/json"); //consumes
    texttospeech_apiClient_invoke(apiClient,
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
    if (localVarSingleItemJSON__on_new_audio_post_request) {
        cJSON_Delete(localVarSingleItemJSON__on_new_audio_post_request);
        localVarSingleItemJSON__on_new_audio_post_request = NULL;
    }
    free(localVarBodyParameters);

}

// Provide text to the TextToSpeech component
//
// Provide text to the TextToSpeech component
//
void
Texttospeech_provideText(texttospeech_apiClient_t *apiClient, provide_text_request_t *provide_text_request)
{
    list_t    *localVarQueryParameters = NULL;
    list_t    *localVarHeaderParameters = NULL;
    list_t    *localVarFormParameters = NULL;
    list_t *localVarHeaderType = list_createList();
    list_t *localVarContentType = list_createList();
    char      *localVarBodyParameters = NULL;

    // create the path
    long sizeOfPath = strlen("/provide_text")+1;
    char *localVarPath = malloc(sizeOfPath);
    snprintf(localVarPath, sizeOfPath, "/provide_text");




    // Body Param
    cJSON *localVarSingleItemJSON_provide_text_request = NULL;
    if (provide_text_request != NULL)
    {
        //string
        localVarSingleItemJSON_provide_text_request = provide_text_request_convertToJSON(provide_text_request);
        localVarBodyParameters = cJSON_Print(localVarSingleItemJSON_provide_text_request);
    }
    list_addElement(localVarHeaderType,"application/json"); //produces
    list_addElement(localVarContentType,"application/json"); //consumes
    texttospeech_apiClient_invoke(apiClient,
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
    
    
    
    list_freeList(localVarHeaderType);
    list_freeList(localVarContentType);
    free(localVarPath);
    if (localVarSingleItemJSON_provide_text_request) {
        cJSON_Delete(localVarSingleItemJSON_provide_text_request);
        localVarSingleItemJSON_provide_text_request = NULL;
    }
    free(localVarBodyParameters);

}

