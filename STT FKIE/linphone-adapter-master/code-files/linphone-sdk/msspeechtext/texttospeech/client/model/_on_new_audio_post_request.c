#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "_on_new_audio_post_request.h"



static _on_new_audio_post_request_t *_on_new_audio_post_request_create_internal(
    int length,
    int sample_rate_hz,
    long timestamp,
    char *message_id,
    char *byte_array
    ) {
    _on_new_audio_post_request_t *_on_new_audio_post_request_local_var = malloc(sizeof(_on_new_audio_post_request_t));
    if (!_on_new_audio_post_request_local_var) {
        return NULL;
    }
    _on_new_audio_post_request_local_var->length = length;
    _on_new_audio_post_request_local_var->sample_rate_hz = sample_rate_hz;
    _on_new_audio_post_request_local_var->timestamp = timestamp;
    _on_new_audio_post_request_local_var->message_id = message_id;
    _on_new_audio_post_request_local_var->byte_array = byte_array;

    return _on_new_audio_post_request_local_var;
}

__attribute__((deprecated)) _on_new_audio_post_request_t *_on_new_audio_post_request_create(
    int length,
    int sample_rate_hz,
    long timestamp,
    char *message_id,
    char *byte_array
    ) {
    return _on_new_audio_post_request_create_internal (
        length,
        sample_rate_hz,
        timestamp,
        message_id,
        byte_array
        );
}

void _on_new_audio_post_request_free(_on_new_audio_post_request_t *_on_new_audio_post_request) {
    if(NULL == _on_new_audio_post_request){
        return ;
    }
    listEntry_t *listEntry;
    if (_on_new_audio_post_request->message_id) {
        free(_on_new_audio_post_request->message_id);
        _on_new_audio_post_request->message_id = NULL;
    }
    if (_on_new_audio_post_request->byte_array) {
        free(_on_new_audio_post_request->byte_array);
        _on_new_audio_post_request->byte_array = NULL;
    }
    free(_on_new_audio_post_request);
}

cJSON *_on_new_audio_post_request_convertToJSON(_on_new_audio_post_request_t *_on_new_audio_post_request) {
    cJSON *item = cJSON_CreateObject();

    // _on_new_audio_post_request->length
    if(_on_new_audio_post_request->length) {
    if(cJSON_AddNumberToObject(item, "length", _on_new_audio_post_request->length) == NULL) {
    goto fail; //Numeric
    }
    }


    // _on_new_audio_post_request->sample_rate_hz
    if(_on_new_audio_post_request->sample_rate_hz) {
    if(cJSON_AddNumberToObject(item, "sample_rate_hz", _on_new_audio_post_request->sample_rate_hz) == NULL) {
    goto fail; //Numeric
    }
    }


    // _on_new_audio_post_request->timestamp
    if(_on_new_audio_post_request->timestamp) {
    if(cJSON_AddNumberToObject(item, "timestamp", _on_new_audio_post_request->timestamp) == NULL) {
    goto fail; //Numeric
    }
    }


    // _on_new_audio_post_request->message_id
    if(_on_new_audio_post_request->message_id) {
    if(cJSON_AddStringToObject(item, "message_id", _on_new_audio_post_request->message_id) == NULL) {
    goto fail; //String
    }
    }


    // _on_new_audio_post_request->byte_array
    if(_on_new_audio_post_request->byte_array) {
    if(cJSON_AddStringToObject(item, "byte_array", _on_new_audio_post_request->byte_array) == NULL) {
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

_on_new_audio_post_request_t *_on_new_audio_post_request_parseFromJSON(cJSON *_on_new_audio_post_requestJSON){

    _on_new_audio_post_request_t *_on_new_audio_post_request_local_var = NULL;

    // _on_new_audio_post_request->length
    cJSON *length = cJSON_GetObjectItemCaseSensitive(_on_new_audio_post_requestJSON, "length");
    if (cJSON_IsNull(length)) {
        length = NULL;
    }
    if (length) { 
    if(!cJSON_IsNumber(length))
    {
    goto end; //Numeric
    }
    }

    // _on_new_audio_post_request->sample_rate_hz
    cJSON *sample_rate_hz = cJSON_GetObjectItemCaseSensitive(_on_new_audio_post_requestJSON, "sample_rate_hz");
    if (cJSON_IsNull(sample_rate_hz)) {
        sample_rate_hz = NULL;
    }
    if (sample_rate_hz) { 
    if(!cJSON_IsNumber(sample_rate_hz))
    {
    goto end; //Numeric
    }
    }

    // _on_new_audio_post_request->timestamp
    cJSON *timestamp = cJSON_GetObjectItemCaseSensitive(_on_new_audio_post_requestJSON, "timestamp");
    if (cJSON_IsNull(timestamp)) {
        timestamp = NULL;
    }
    if (timestamp) { 
    if(!cJSON_IsNumber(timestamp))
    {
    goto end; //Numeric
    }
    }

    // _on_new_audio_post_request->message_id
    cJSON *message_id = cJSON_GetObjectItemCaseSensitive(_on_new_audio_post_requestJSON, "message_id");
    if (cJSON_IsNull(message_id)) {
        message_id = NULL;
    }
    if (message_id) { 
    if(!cJSON_IsString(message_id) && !cJSON_IsNull(message_id))
    {
    goto end; //String
    }
    }

    // _on_new_audio_post_request->byte_array
    cJSON *byte_array = cJSON_GetObjectItemCaseSensitive(_on_new_audio_post_requestJSON, "byte_array");
    if (cJSON_IsNull(byte_array)) {
        byte_array = NULL;
    }
    if (byte_array) { 
    if(!cJSON_IsString(byte_array))
    {
    goto end; //ByteArray
    }
    }


    _on_new_audio_post_request_local_var = _on_new_audio_post_request_create_internal (
        length ? length->valuedouble : 0,
        sample_rate_hz ? sample_rate_hz->valuedouble : 0,
        timestamp ? timestamp->valuedouble : 0,
        message_id && !cJSON_IsNull(message_id) ? strdup(message_id->valuestring) : NULL,
        byte_array ? strdup(byte_array->valuestring) : NULL
        );

    return _on_new_audio_post_request_local_var;
end:
    return NULL;

}
