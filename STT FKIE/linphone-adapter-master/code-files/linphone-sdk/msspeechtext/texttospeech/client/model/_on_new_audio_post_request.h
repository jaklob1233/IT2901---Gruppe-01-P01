/*
 * _on_new_audio_post_request.h
 *
 * 
 */

#ifndef __on_new_audio_post_request_H_
#define __on_new_audio_post_request_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct _on_new_audio_post_request_t _on_new_audio_post_request_t;




typedef struct _on_new_audio_post_request_t {
    int length; //numeric
    int sample_rate_hz; //numeric
    long timestamp; //numeric
    char *message_id; // string
    char *byte_array; //ByteArray

} _on_new_audio_post_request_t;

_on_new_audio_post_request_t *_on_new_audio_post_request_create(
    int length,
    int sample_rate_hz,
    long timestamp,
    char *message_id,
    char *byte_array
);

void _on_new_audio_post_request_free(_on_new_audio_post_request_t *_on_new_audio_post_request);

_on_new_audio_post_request_t *_on_new_audio_post_request_parseFromJSON(cJSON *_on_new_audio_post_requestJSON);

cJSON *_on_new_audio_post_request_convertToJSON(_on_new_audio_post_request_t *_on_new_audio_post_request);

#endif /* __on_new_audio_post_request_H_ */

