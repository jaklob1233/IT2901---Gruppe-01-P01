#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "message.h"



message_t *message_create(
    char *sender_id,
    char *receiver_id,
    char *message_id,
    char *message,
    long timestamp
    ) {
    message_t *message_local_var = malloc(sizeof(message_t));
    if (!message_local_var) {
        return NULL;
    }
    message_local_var->sender_id = sender_id;
    message_local_var->receiver_id = receiver_id;
    message_local_var->message_id = message_id;
    message_local_var->message = message;
    message_local_var->timestamp = timestamp;

    return message_local_var;
}


void message_free(message_t *message) {
    if(NULL == message){
        return ;
    }
    listEntry_t *listEntry;
    if (message->sender_id) {
        free(message->sender_id);
        message->sender_id = NULL;
    }
    if (message->receiver_id) {
        free(message->receiver_id);
        message->receiver_id = NULL;
    }
    if (message->message_id) {
        free(message->message_id);
        message->message_id = NULL;
    }
    if (message->message) {
        free(message->message);
        message->message = NULL;
    }
    free(message);
}

cJSON *message_convertToJSON(message_t *message) {
    cJSON *item = cJSON_CreateObject();

    // message->sender_id
    if (!message->sender_id) {
        goto fail;
    }
    if(cJSON_AddStringToObject(item, "sender_id", message->sender_id) == NULL) {
    goto fail; //String
    }


    // message->receiver_id
    if (!message->receiver_id) {
        goto fail;
    }
    if(cJSON_AddStringToObject(item, "receiver_id", message->receiver_id) == NULL) {
    goto fail; //String
    }


    // message->message_id
    if (!message->message_id) {
        goto fail;
    }
    if(cJSON_AddStringToObject(item, "message_id", message->message_id) == NULL) {
    goto fail; //String
    }


    // message->message
    if (!message->message) {
        goto fail;
    }
    if(cJSON_AddStringToObject(item, "message", message->message) == NULL) {
    goto fail; //String
    }


    // message->timestamp
    if (!message->timestamp) {
        goto fail;
    }
    if(cJSON_AddNumberToObject(item, "timestamp", message->timestamp) == NULL) {
    goto fail; //Numeric
    }

    return item;
fail:
    if (item) {
        cJSON_Delete(item);
    }
    return NULL;
}

message_t *message_parseFromJSON(cJSON *messageJSON){

    message_t *message_local_var = NULL;

    // message->sender_id
    cJSON *sender_id = cJSON_GetObjectItemCaseSensitive(messageJSON, "sender_id");
    if (!sender_id) {
        goto end;
    }

    
    if(!cJSON_IsString(sender_id))
    {
    goto end; //String
    }

    // message->receiver_id
    cJSON *receiver_id = cJSON_GetObjectItemCaseSensitive(messageJSON, "receiver_id");
    if (!receiver_id) {
        goto end;
    }

    
    if(!cJSON_IsString(receiver_id))
    {
    goto end; //String
    }

    // message->message_id
    cJSON *message_id = cJSON_GetObjectItemCaseSensitive(messageJSON, "message_id");
    if (!message_id) {
        goto end;
    }

    
    if(!cJSON_IsString(message_id))
    {
    goto end; //String
    }

    // message->message
    cJSON *message = cJSON_GetObjectItemCaseSensitive(messageJSON, "message");
    if (!message) {
        goto end;
    }

    
    if(!cJSON_IsString(message))
    {
    goto end; //String
    }

    // message->timestamp
    cJSON *timestamp = cJSON_GetObjectItemCaseSensitive(messageJSON, "timestamp");
    if (!timestamp) {
        goto end;
    }

    
    if(!cJSON_IsNumber(timestamp))
    {
    goto end; //Numeric
    }


    message_local_var = message_create (
        strdup(sender_id->valuestring),
        strdup(receiver_id->valuestring),
        strdup(message_id->valuestring),
        strdup(message->valuestring),
        timestamp->valuedouble
        );

    return message_local_var;
end:
    return NULL;

}
