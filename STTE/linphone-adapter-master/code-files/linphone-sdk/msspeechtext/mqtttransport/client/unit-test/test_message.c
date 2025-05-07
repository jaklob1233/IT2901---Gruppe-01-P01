#ifndef message_TEST
#define message_TEST

// the following is to include only the main from the first c file
#ifndef TEST_MAIN
#define TEST_MAIN
#define message_MAIN
#endif // TEST_MAIN

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "../external/cJSON.h"

#include "../model/message.h"
message_t* instantiate_message(int include_optional);



message_t* instantiate_message(int include_optional) {
  message_t* message = NULL;
  if (include_optional) {
    message = message_create(
      "0",
      "0",
      "0",
      56
    );
  } else {
    message = message_create(
      "0",
      "0",
      "0",
      56
    );
  }

  return message;
}


#ifdef message_MAIN

void test_message(int include_optional) {
    message_t* message_1 = instantiate_message(include_optional);

	cJSON* jsonmessage_1 = message_convertToJSON(message_1);
	printf("message :\n%s\n", cJSON_Print(jsonmessage_1));
	message_t* message_2 = message_parseFromJSON(jsonmessage_1);
	cJSON* jsonmessage_2 = message_convertToJSON(message_2);
	printf("repeating message:\n%s\n", cJSON_Print(jsonmessage_2));
}

int main() {
  test_message(1);
  test_message(0);

  printf("Hello world \n");
  return 0;
}

#endif // message_MAIN
#endif // message_TEST
