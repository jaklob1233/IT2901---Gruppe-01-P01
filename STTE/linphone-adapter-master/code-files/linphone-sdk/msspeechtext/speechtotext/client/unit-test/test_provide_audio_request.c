#ifndef provide_audio_request_TEST
#define provide_audio_request_TEST

// the following is to include only the main from the first c file
#ifndef TEST_MAIN
#define TEST_MAIN
#define provide_audio_request_MAIN
#endif // TEST_MAIN

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../external/cJSON.h"

#include "../model/provide_audio_request.h"
provide_audio_request_t* instantiate_provide_audio_request(int include_optional);



provide_audio_request_t* instantiate_provide_audio_request(int include_optional) {
  provide_audio_request_t* provide_audio_request = NULL;
  if (include_optional) {
    provide_audio_request = provide_audio_request_create(
      56,
      56,
      56,
      "YQ=="
    );
  } else {
    provide_audio_request = provide_audio_request_create(
      56,
      56,
      56,
      "YQ=="
    );
  }

  return provide_audio_request;
}


#ifdef provide_audio_request_MAIN

void test_provide_audio_request(int include_optional) {
    provide_audio_request_t* provide_audio_request_1 = instantiate_provide_audio_request(include_optional);

	cJSON* jsonprovide_audio_request_1 = provide_audio_request_convertToJSON(provide_audio_request_1);
	printf("provide_audio_request :\n%s\n", cJSON_Print(jsonprovide_audio_request_1));
	provide_audio_request_t* provide_audio_request_2 = provide_audio_request_parseFromJSON(jsonprovide_audio_request_1);
	cJSON* jsonprovide_audio_request_2 = provide_audio_request_convertToJSON(provide_audio_request_2);
	printf("repeating provide_audio_request:\n%s\n", cJSON_Print(jsonprovide_audio_request_2));
}

int main() {
  test_provide_audio_request(1);
  test_provide_audio_request(0);

  printf("Hello world \n");
  return 0;
}

#endif // provide_audio_request_MAIN
#endif // provide_audio_request_TEST
