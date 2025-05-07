#include <stdlib.h>
#include <stdio.h>
#include "../include/apiClient.h"
#include "../include/list.h"
#include "../external/cJSON.h"
#include "../include/keyValuePair.h"
#include "../include/binary.h"
#include "../model/_on_new_audio_post_request.h"
#include "../model/error.h"
#include "../model/provide_text_request.h"

// Enum TEXTTOSPEECHVARIANT for DefaultAPI_initialize
typedef enum  { texttospeech_initialize_TEXTTOSPEECHVARIANT_NULL = 0, texttospeech_initialize_TEXTTOSPEECHVARIANT_standard, texttospeech_initialize_TEXTTOSPEECHVARIANT_coqui_yourtts, texttospeech_initialize_TEXTTOSPEECHVARIANT_coqui_xtts } texttospeech_initialize_texttospeech_variant_e;


// Initialize the TextToSpeech component
//
// Initialize the TextToSpeech component
//
void
Texttospeech_initialize(texttospeech_apiClient_t *apiClient, texttospeech_initialize_texttospeech_variant_e texttospeech_variant, char *config_profile, char *webhook_url);


void
Texttospeech_onNewAudioPost(texttospeech_apiClient_t *apiClient, _on_new_audio_post_request_t *_on_new_audio_post_request);


// Provide text to the TextToSpeech component
//
// Provide text to the TextToSpeech component
//
void
Texttospeech_provideText(texttospeech_apiClient_t *apiClient, provide_text_request_t *provide_text_request);


