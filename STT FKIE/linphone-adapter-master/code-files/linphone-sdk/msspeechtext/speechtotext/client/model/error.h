/*
 * error.h
 *
 * 
 */

#ifndef _error_H_
#define _error_H_

#include <string.h>
#include "../external/cJSON.h"
#include "../include/list.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"

typedef struct error_t_2 error_t_2;

#include "object.h"



typedef struct error_t_2 {
    char *error_message; // string
    object_t *detail; //object

} error_t_2;

error_t_2 *error_create(
    char *error_message,
    object_t *detail
);

void error_free(error_t_2 *error);

error_t_2 *error_parseFromJSON(cJSON *errorJSON);

cJSON *error_convertToJSON(error_t_2 *error);

#endif /* _error_H_ */

