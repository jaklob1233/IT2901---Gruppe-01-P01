/*
 * _on_new_text_post_request.h
 *
 * 
 */

#ifndef __on_new_text_post_request_H_
#define __on_new_text_post_request_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct _on_new_text_post_request_t _on_new_text_post_request_t;




typedef struct _on_new_text_post_request_t {
    long timestamp_start; //numeric
    long timestamp_end; //numeric
    char *text; // string

} _on_new_text_post_request_t;

_on_new_text_post_request_t *_on_new_text_post_request_create(
    long timestamp_start,
    long timestamp_end,
    char *text
);

void _on_new_text_post_request_free(_on_new_text_post_request_t *_on_new_text_post_request);

_on_new_text_post_request_t *_on_new_text_post_request_parseFromJSON(cJSON *_on_new_text_post_requestJSON);

cJSON *_on_new_text_post_request_convertToJSON(_on_new_text_post_request_t *_on_new_text_post_request);

#endif /* __on_new_text_post_request_H_ */

