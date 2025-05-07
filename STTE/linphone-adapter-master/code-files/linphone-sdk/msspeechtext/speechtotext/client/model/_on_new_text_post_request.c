#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "_on_new_text_post_request.h"



_on_new_text_post_request_t *_on_new_text_post_request_create(
    long timestamp_start,
    long timestamp_end,
    char *text
    ) {
    _on_new_text_post_request_t *_on_new_text_post_request_local_var = malloc(sizeof(_on_new_text_post_request_t));
    if (!_on_new_text_post_request_local_var) {
        return NULL;
    }
    _on_new_text_post_request_local_var->timestamp_start = timestamp_start;
    _on_new_text_post_request_local_var->timestamp_end = timestamp_end;
    _on_new_text_post_request_local_var->text = text;

    return _on_new_text_post_request_local_var;
}


void _on_new_text_post_request_free(_on_new_text_post_request_t *_on_new_text_post_request) {
    if(NULL == _on_new_text_post_request){
        return ;
    }
    listEntry_t *listEntry;
    if (_on_new_text_post_request->text) {
        free(_on_new_text_post_request->text);
        _on_new_text_post_request->text = NULL;
    }
    free(_on_new_text_post_request);
}

cJSON *_on_new_text_post_request_convertToJSON(_on_new_text_post_request_t *_on_new_text_post_request) {
    cJSON *item = cJSON_CreateObject();

    // _on_new_text_post_request->timestamp_start
    if(_on_new_text_post_request->timestamp_start) {
    if(cJSON_AddNumberToObject(item, "timestamp_start", _on_new_text_post_request->timestamp_start) == NULL) {
    goto fail; //Numeric
    }
    }


    // _on_new_text_post_request->timestamp_end
    if(_on_new_text_post_request->timestamp_end) {
    if(cJSON_AddNumberToObject(item, "timestamp_end", _on_new_text_post_request->timestamp_end) == NULL) {
    goto fail; //Numeric
    }
    }


    // _on_new_text_post_request->text
    if(_on_new_text_post_request->text) {
    if(cJSON_AddStringToObject(item, "text", _on_new_text_post_request->text) == NULL) {
    goto fail; //String
    }
    }

    return item;
fail:
    if (item) {
        cJSON_Delete(item);
    }
    return NULL;
}

_on_new_text_post_request_t *_on_new_text_post_request_parseFromJSON(cJSON *_on_new_text_post_requestJSON){

    _on_new_text_post_request_t *_on_new_text_post_request_local_var = NULL;

    // _on_new_text_post_request->timestamp_start
    cJSON *timestamp_start = cJSON_GetObjectItemCaseSensitive(_on_new_text_post_requestJSON, "timestamp_start");
    if (timestamp_start) { 
    if(!cJSON_IsNumber(timestamp_start))
    {
    goto end; //Numeric
    }
    }

    // _on_new_text_post_request->timestamp_end
    cJSON *timestamp_end = cJSON_GetObjectItemCaseSensitive(_on_new_text_post_requestJSON, "timestamp_end");
    if (timestamp_end) { 
    if(!cJSON_IsNumber(timestamp_end))
    {
    goto end; //Numeric
    }
    }

    // _on_new_text_post_request->text
    cJSON *text = cJSON_GetObjectItemCaseSensitive(_on_new_text_post_requestJSON, "text");
    if (text) { 
    if(!cJSON_IsString(text) && !cJSON_IsNull(text))
    {
    goto end; //String
    }
    }


    _on_new_text_post_request_local_var = _on_new_text_post_request_create (
        timestamp_start ? timestamp_start->valuedouble : 0,
        timestamp_end ? timestamp_end->valuedouble : 0,
        text && !cJSON_IsNull(text) ? strdup(text->valuestring) : NULL
        );

    return _on_new_text_post_request_local_var;
end:
    return NULL;

}
