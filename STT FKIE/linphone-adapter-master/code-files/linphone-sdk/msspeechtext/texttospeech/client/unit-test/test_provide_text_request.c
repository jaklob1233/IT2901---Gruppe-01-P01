#ifndef provide_text_request_TEST
#define provide_text_request_TEST

// the following is to include only the main from the first c file
#ifndef TEST_MAIN
#define TEST_MAIN
#define provide_text_request_MAIN
#endif // TEST_MAIN

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../external/cJSON.h"

#include "../model/provide_text_request.h"
provide_text_request_t* instantiate_provide_text_request(int include_optional);



provide_text_request_t* instantiate_provide_text_request(int include_optional) {
  provide_text_request_t* provide_text_request = NULL;
  if (include_optional) {
    provide_text_request = provide_text_request_create(
      56,
      56,
      "0",
      "0"
    );
  } else {
    provide_text_request = provide_text_request_create(
      56,
      56,
      "0",
      "0"
    );
  }

  return provide_text_request;
}


#ifdef provide_text_request_MAIN

void test_provide_text_request(int include_optional) {
    provide_text_request_t* provide_text_request_1 = instantiate_provide_text_request(include_optional);

	cJSON* jsonprovide_text_request_1 = provide_text_request_convertToJSON(provide_text_request_1);
	printf("provide_text_request :\n%s\n", cJSON_Print(jsonprovide_text_request_1));
	provide_text_request_t* provide_text_request_2 = provide_text_request_parseFromJSON(jsonprovide_text_request_1);
	cJSON* jsonprovide_text_request_2 = provide_text_request_convertToJSON(provide_text_request_2);
	printf("repeating provide_text_request:\n%s\n", cJSON_Print(jsonprovide_text_request_2));
}

int main() {
  test_provide_text_request(1);
  test_provide_text_request(0);

  printf("Hello world \n");
  return 0;
}

#endif // provide_text_request_MAIN
#endif // provide_text_request_TEST
