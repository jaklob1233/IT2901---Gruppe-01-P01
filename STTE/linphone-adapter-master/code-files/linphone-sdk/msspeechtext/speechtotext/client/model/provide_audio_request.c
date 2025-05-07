#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "provide_audio_request.h"



provide_audio_request_t *provide_audio_request_create(
    int length,
    int sample_rate_hz,
    long timestamp,
    char *byte_array
    ) {
    provide_audio_request_t *provide_audio_request_local_var = malloc(sizeof(provide_audio_request_t));
    if (!provide_audio_request_local_var) {
        return NULL;
    }
    provide_audio_request_local_var->length = length;
    provide_audio_request_local_var->sample_rate_hz = sample_rate_hz;
    provide_audio_request_local_var->timestamp = timestamp;
    provide_audio_request_local_var->byte_array = byte_array;

    return provide_audio_request_local_var;
}


void provide_audio_request_free(provide_audio_request_t *provide_audio_request) {
    if(NULL == provide_audio_request){
        return ;
    }
    listEntry_t *listEntry;
    if (provide_audio_request->byte_array) {
        free(provide_audio_request->byte_array);
        provide_audio_request->byte_array = NULL;
    }
    free(provide_audio_request);
}

cJSON *provide_audio_request_convertToJSON(provide_audio_request_t *provide_audio_request) {
    cJSON *item = cJSON_CreateObject();

    // provide_audio_request->length
    if(provide_audio_request->length) {
    if(cJSON_AddNumberToObject(item, "length", provide_audio_request->length) == NULL) {
    goto fail; //Numeric
    }
    }


    // provide_audio_request->sample_rate_hz
    if(provide_audio_request->sample_rate_hz) {
    if(cJSON_AddNumberToObject(item, "sample_rate_hz", provide_audio_request->sample_rate_hz) == NULL) {
    goto fail; //Numeric
    }
    }


    // provide_audio_request->timestamp
    if(provide_audio_request->timestamp) {
    if(cJSON_AddNumberToObject(item, "timestamp", provide_audio_request->timestamp) == NULL) {
    goto fail; //Numeric
    }
    }


    // provide_audio_request->byte_array
    if(provide_audio_request->byte_array) {
    if(cJSON_AddStringToObject(item, "byte_array", provide_audio_request->byte_array) == NULL) {
    goto fail; //ByteArray
    }
    }

    return item;
fail:
    if (item) {
        cJSON_Delete(item);
    }
    return NULL;
}

provide_audio_request_t *provide_audio_request_parseFromJSON(cJSON *provide_audio_requestJSON){

    provide_audio_request_t *provide_audio_request_local_var = NULL;

    // provide_audio_request->length
    cJSON *length = cJSON_GetObjectItemCaseSensitive(provide_audio_requestJSON, "length");
    if (length) { 
    if(!cJSON_IsNumber(length))
    {
    goto end; //Numeric
    }
    }

    // provide_audio_request->sample_rate_hz
    cJSON *sample_rate_hz = cJSON_GetObjectItemCaseSensitive(provide_audio_requestJSON, "sample_rate_hz");
    if (sample_rate_hz) { 
    if(!cJSON_IsNumber(sample_rate_hz))
    {
    goto end; //Numeric
    }
    }

    // provide_audio_request->timestamp
    cJSON *timestamp = cJSON_GetObjectItemCaseSensitive(provide_audio_requestJSON, "timestamp");
    if (timestamp) { 
    if(!cJSON_IsNumber(timestamp))
    {
    goto end; //Numeric
    }
    }

    // provide_audio_request->byte_array
    cJSON *byte_array = cJSON_GetObjectItemCaseSensitive(provide_audio_requestJSON, "byte_array");
    if (byte_array) { 
    if(!cJSON_IsString(byte_array))
    {
    goto end; //ByteArray
    }
    }


    provide_audio_request_local_var = provide_audio_request_create (
        length ? length->valuedouble : 0,
        sample_rate_hz ? sample_rate_hz->valuedouble : 0,
        timestamp ? timestamp->valuedouble : 0,
        byte_array ? strdup(byte_array->valuestring) : NULL
        );

    return provide_audio_request_local_var;
end:
    return NULL;

}
