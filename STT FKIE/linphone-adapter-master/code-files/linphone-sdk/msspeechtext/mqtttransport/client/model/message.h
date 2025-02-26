/*
 * message.h
 *
 * 
 */

#ifndef _message_H_
#define _message_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct message_t message_t;




typedef struct message_t {
    char *sender_id; // string
    char *receiver_id; // string
    char *message_id; // string
    char *message; // string
    long timestamp; //numeric

} message_t;

message_t *message_create(
    char *sender_id,
    char *receiver_id,
    char *message_id,
    char *message,
    long timestamp
);

void message_free(message_t *message);

message_t *message_parseFromJSON(cJSON *messageJSON);

cJSON *message_convertToJSON(message_t *message);

#endif /* _message_H_ */

