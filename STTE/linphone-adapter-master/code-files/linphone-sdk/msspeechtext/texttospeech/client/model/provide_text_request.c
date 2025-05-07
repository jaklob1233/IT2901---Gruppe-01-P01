#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "provide_text_request.h"



provide_text_request_t *provide_text_request_create(
    long timestamp_start,
    long timestamp_end,
    char *message_id,
    char *text
    ) {
    provide_text_request_t *provide_text_request_local_var = malloc(sizeof(provide_text_request_t));
    if (!provide_text_request_local_var) {
        return NULL;
    }
    provide_text_request_local_var->timestamp_start = timestamp_start;
    provide_text_request_local_var->timestamp_end = timestamp_end;
    provide_text_request_local_var->message_id = message_id;
    provide_text_request_local_var->text = text;

    return provide_text_request_local_var;
}


void provide_text_request_free(provide_text_request_t *provide_text_request) {
    if(NULL == provide_text_request){
        return ;
    }
    listEntry_t *listEntry;
    if (provide_text_request->message_id) {
        free(provide_text_request->message_id);
        provide_text_request->message_id = NULL;
    }
    if (provide_text_request->text) {
        free(provide_text_request->text);
        provide_text_request->text = NULL;
    }
    free(provide_text_request);
}

cJSON *provide_text_request_convertToJSON(provide_text_request_t *provide_text_request) {
    cJSON *item = cJSON_CreateObject();

    // provide_text_request->timestamp_start
    if(provide_text_request->timestamp_start) {
    if(cJSON_AddNumberToObject(item, "timestamp_start", provide_text_request->timestamp_start) == NULL) {
    goto fail; //Numeric
    }
    }


    // provide_text_request->timestamp_end
    if(provide_text_request->timestamp_end) {
    if(cJSON_AddNumberToObject(item, "timestamp_end", provide_text_request->timestamp_end) == NULL) {
    goto fail; //Numeric
    }
    }


    // provide_text_request->message_id
    if(provide_text_request->message_id) {
    if(cJSON_AddStringToObject(item, "message_id", provide_text_request->message_id) == NULL) {
    goto fail; //String
    }
    }


    // provide_text_request->text
    if(provide_text_request->text) {
    if(cJSON_AddStringToObject(item, "text", provide_text_request->text) == NULL) {
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

provide_text_request_t *provide_text_request_parseFromJSON(cJSON *provide_text_requestJSON){

    provide_text_request_t *provide_text_request_local_var = NULL;

    // provide_text_request->timestamp_start
    cJSON *timestamp_start = cJSON_GetObjectItemCaseSensitive(provide_text_requestJSON, "timestamp_start");
    if (cJSON_IsNull(timestamp_start)) {
        timestamp_start = NULL;
    }
    if (timestamp_start) { 
    if(!cJSON_IsNumber(timestamp_start))
    {
    goto end; //Numeric
    }
    }

    // provide_text_request->timestamp_end
    cJSON *timestamp_end = cJSON_GetObjectItemCaseSensitive(provide_text_requestJSON, "timestamp_end");
    if (cJSON_IsNull(timestamp_end)) {
        timestamp_end = NULL;
    }
    if (timestamp_end) { 
    if(!cJSON_IsNumber(timestamp_end))
    {
    goto end; //Numeric
    }
    }

    // provide_text_request->message_id
    cJSON *message_id = cJSON_GetObjectItemCaseSensitive(provide_text_requestJSON, "message_id");
    if (cJSON_IsNull(message_id)) {
        message_id = NULL;
    }
    if (message_id) { 
    if(!cJSON_IsString(message_id) && !cJSON_IsNull(message_id))
    {
    goto end; //String
    }
    }

    // provide_text_request->text
    cJSON *text = cJSON_GetObjectItemCaseSensitive(provide_text_requestJSON, "text");
    if (cJSON_IsNull(text)) {
        text = NULL;
    }
    if (text) { 
    if(!cJSON_IsString(text) && !cJSON_IsNull(text))
    {
    goto end; //String
    }
    }


    provide_text_request_local_var = provide_text_request_create (
        timestamp_start ? timestamp_start->valuedouble : 0,
        timestamp_end ? timestamp_end->valuedouble : 0,
        message_id && !cJSON_IsNull(message_id) ? strdup(message_id->valuestring) : NULL,
        text && !cJSON_IsNull(text) ? strdup(text->valuestring) : NULL
        );

    return provide_text_request_local_var;
end:
    return NULL;

}
