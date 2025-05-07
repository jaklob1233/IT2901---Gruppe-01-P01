#include <stdlib.h>
#include <stdio.h>
#include "../include/apiClient.h"
#include "../include/list.h"
#include "../external/cJSON.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"
#include "../model/_on_new_text_post_request.h"
#include "../model/error.h"
#include "../model/provide_audio_request.h"

// Enum SPEECHTOTEXTVARIANT for DefaultAPI_initialize
typedef enum  { speechtotext_initialize_SPEECHTOTEXTVARIANT_NULL = 0, speechtotext_initialize_SPEECHTOTEXTVARIANT_Whisper, speechtotext_initialize_SPEECHTOTEXTVARIANT_Vosk } speechtotext_initialize_speechtotext_variant_e;


// Initialize the SpeechToText component
//
// Initialize the SpeechToText component
//
void
Speechtotext_initialize(speechtotext_apiClient_t *apiClient, speechtotext_initialize_speechtotext_variant_e speechtotext_variant, char *config_profile, char *webhook_url);


void
Speechtotext_onNewTextPost(speechtotext_apiClient_t *apiClient, _on_new_text_post_request_t *_on_new_text_post_request);


// Provide audio to the SpeechToText component
//
// Provide audio to the SpeechToText component
//
void
Speechtotext_provideAudio(speechtotext_apiClient_t *apiClient, provide_audio_request_t *provide_audio_request);


