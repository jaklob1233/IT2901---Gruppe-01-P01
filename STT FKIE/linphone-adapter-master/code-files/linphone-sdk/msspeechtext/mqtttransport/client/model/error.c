#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "error.h"



error_t *error_create(
    char *error_message,
    object_t *detail
    ) {
    error_t *error_local_var = malloc(sizeof(error_t));
    if (!error_local_var) {
        return NULL;
    }
    error_local_var->error_message = error_message;
    error_local_var->detail = detail;

    return error_local_var;
}


void error_free(error_t *error) {
    if(NULL == error){
        return ;
    }
    listEntry_t *listEntry;
    if (error->error_message) {
        free(error->error_message);
        error->error_message = NULL;
    }
    if (error->detail) {
        object_free(error->detail);
        error->detail = NULL;
    }
    free(error);
}

cJSON *error_convertToJSON(error_t *error) {
    cJSON *item = cJSON_CreateObject();

    // error->error_message
    if(error->error_message) {
    if(cJSON_AddStringToObject(item, "error_message", error->error_message) == NULL) {
    goto fail; //String
    }
    }


    // error->detail
    if(error->detail) {
    cJSON *detail_object = object_convertToJSON(error->detail);
    if(detail_object == NULL) {
    goto fail; //model
    }
    cJSON_AddItemToObject(item, "detail", detail_object);
    if(item->child == NULL) {
    goto fail;
    }
    }

    return item;
fail:
    if (item) {
        cJSON_Delete(item);
    }
    return NULL;
}

error_t *error_parseFromJSON(cJSON *errorJSON){

    error_t *error_local_var = NULL;

    // error->error_message
    cJSON *error_message = cJSON_GetObjectItemCaseSensitive(errorJSON, "error_message");
    if (error_message) { 
    if(!cJSON_IsString(error_message) && !cJSON_IsNull(error_message))
    {
    goto end; //String
    }
    }

    // error->detail
    cJSON *detail = cJSON_GetObjectItemCaseSensitive(errorJSON, "detail");
    object_t *detail_local_object = NULL;
    if (detail) { 
    detail_local_object = object_parseFromJSON(detail); //object
    }


    error_local_var = error_create (
        error_message && !cJSON_IsNull(error_message) ? strdup(error_message->valuestring) : NULL,
        detail ? detail_local_object : NULL
        );

    return error_local_var;
end:
    return NULL;

}
