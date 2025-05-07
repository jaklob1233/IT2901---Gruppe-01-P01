#ifndef _on_new_audio_post_request_TEST
#define _on_new_audio_post_request_TEST

// the following is to include only the main from the first c file
#ifndef TEST_MAIN
#define TEST_MAIN
#define _on_new_audio_post_request_MAIN
#endif // TEST_MAIN

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../external/cJSON.h"

#include "../model/_on_new_audio_post_request.h"
_on_new_audio_post_request_t* instantiate__on_new_audio_post_request(int include_optional);



_on_new_audio_post_request_t* instantiate__on_new_audio_post_request(int include_optional) {
  _on_new_audio_post_request_t* _on_new_audio_post_request = NULL;
  if (include_optional) {
    _on_new_audio_post_request = _on_new_audio_post_request_create(
      640   # 16000 * 0,02 * 2 (16 kHz, 20 ms, 2 Bytes=16 Bit,
      16000,
      56,
      "0",
      "YQ=="
    );
  } else {
    _on_new_audio_post_request = _on_new_audio_post_request_create(
      640   # 16000 * 0,02 * 2 (16 kHz, 20 ms, 2 Bytes=16 Bit,
      16000,
      56,
      "0",
      "YQ=="
    );
  }

  return _on_new_audio_post_request;
}


#ifdef _on_new_audio_post_request_MAIN

void test__on_new_audio_post_request(int include_optional) {
    _on_new_audio_post_request_t* _on_new_audio_post_request_1 = instantiate__on_new_audio_post_request(include_optional);

	cJSON* json_on_new_audio_post_request_1 = _on_new_audio_post_request_convertToJSON(_on_new_audio_post_request_1);
	printf("_on_new_audio_post_request :\n%s\n", cJSON_Print(json_on_new_audio_post_request_1));
	_on_new_audio_post_request_t* _on_new_audio_post_request_2 = _on_new_audio_post_request_parseFromJSON(json_on_new_audio_post_request_1);
	cJSON* json_on_new_audio_post_request_2 = _on_new_audio_post_request_convertToJSON(_on_new_audio_post_request_2);
	printf("repeating _on_new_audio_post_request:\n%s\n", cJSON_Print(json_on_new_audio_post_request_2));
}

int main() {
  test__on_new_audio_post_request(1);
  test__on_new_audio_post_request(0);

  printf("Hello world \n");
  return 0;
}

#endif // _on_new_audio_post_request_MAIN
#endif // _on_new_audio_post_request_TEST
