/*
 * provide_audio_request.h
 *
 * 
 */

#ifndef _provide_audio_request_H_
#define _provide_audio_request_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct provide_audio_request_t provide_audio_request_t;




typedef struct provide_audio_request_t {
    int length; //numeric
    int sample_rate_hz; //numeric
    long timestamp; //numeric
    char *byte_array; //ByteArray

} provide_audio_request_t;

provide_audio_request_t *provide_audio_request_create(
    int length,
    int sample_rate_hz,
    long timestamp,
    char *byte_array
);

void provide_audio_request_free(provide_audio_request_t *provide_audio_request);

provide_audio_request_t *provide_audio_request_parseFromJSON(cJSON *provide_audio_requestJSON);

cJSON *provide_audio_request_convertToJSON(provide_audio_request_t *provide_audio_request);

#endif /* _provide_audio_request_H_ */

