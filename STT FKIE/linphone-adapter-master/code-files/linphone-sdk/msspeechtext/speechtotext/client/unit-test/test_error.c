#ifndef error_TEST
#define error_TEST

// the following is to include only the main from the first c file
#ifndef TEST_MAIN
#define TEST_MAIN
#define error_MAIN
#endif // TEST_MAIN

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../external/cJSON.h"

#include "../model/error.h"
error_t_2* instantiate_error(int include_optional);



error_t_2* instantiate_error(int include_optional) {
  error_t_2* error = NULL;
  if (include_optional) {
    error = error_create(
      "0",
      0
    );
  } else {
    error = error_create(
      "0",
      0
    );
  }

  return error;
}


#ifdef error_MAIN

void test_error(int include_optional) {
    error_t_2* error_1 = instantiate_error(include_optional);

	cJSON* jsonerror_1 = error_convertToJSON(error_1);
	printf("error :\n%s\n", cJSON_Print(jsonerror_1));
	error_t_2* error_2 = error_parseFromJSON(jsonerror_1);
	cJSON* jsonerror_2 = error_convertToJSON(error_2);
	printf("repeating error:\n%s\n", cJSON_Print(jsonerror_2));
}

int main() {
  test_error(1);
  test_error(0);

  printf("Hello world \n");
  return 0;
}

#endif // error_MAIN
#endif // error_TEST
