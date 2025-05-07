/*
 * provide_text_request.h
 *
 * 
 */

#ifndef _provide_text_request_H_
#define _provide_text_request_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct provide_text_request_t provide_text_request_t;




typedef struct provide_text_request_t {
    long timestamp_start; //numeric
    long timestamp_end; //numeric
    char *message_id; // string
    char *text; // string

} provide_text_request_t;

provide_text_request_t *provide_text_request_create(
    long timestamp_start,
    long timestamp_end,
    char *message_id,
    char *text
);

void provide_text_request_free(provide_text_request_t *provide_text_request);

provide_text_request_t *provide_text_request_parseFromJSON(cJSON *provide_text_requestJSON);

cJSON *provide_text_request_convertToJSON(provide_text_request_t *provide_text_request);

#endif /* _provide_text_request_H_ */

