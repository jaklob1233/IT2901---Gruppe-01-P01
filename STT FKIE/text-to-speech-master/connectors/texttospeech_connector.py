from configobj import ConfigObj
from openapi_client.api_client import TextToSpeechApiClient
from text_to_speech.speaker.speaker import Speaker
import logging
import base64

logger = logging.getLogger(__name__)

class TextToSpeechConnector:

    __speaker: Speaker
    __rest_client = None

    def initialize_texttospeech(self, texttospeech_variant, config_profile, webhook_url):
        logger.info("Initializing texttospeech connector")
        logger.debug(
            f"Received parameters: texttospeech_variant={texttospeech_variant}, config_profile={config_profile}, webhook_url={webhook_url}")
        try:
            logger.debug("Loading configuration from profile")
            config_object = ConfigObj(config_profile)
            logger.debug("Configuration loaded successfully")

            logger.debug("Initializing TextToSpeechApiClient with webhook_url")
            self.__rest_client = TextToSpeechApiClient(webhook_url)
            logger.debug("TextToSpeechApiClient initialized successfully")

            logger.debug("Creating Speaker instance")
            self.__speaker = Speaker(texttospeech_variant, config_object)
            logger.debug("Speaker instance created successfully")

            logger.info("TextToSpeech connector initialized successfully.")
            return True
        except Exception as e:
            logger.error("Failed to initialize texttospeech connector: %s", str(e))
            logger.debug("Exception details:", exc_info=True)
            return False

    def run_texttospeech_on_text(self, text, message_id, timestamp_start, timestamp_end):
        logger.info('Running texttospeech on text: "%s"', text)

        try:
            self.__speaker.accept_data(text)
            snippet = self.__speaker.speak()
            self.__speaker.clear_data()

            logger.debug("Generated audio snippet: %s", snippet.audio)

            # Prepare the payload
            on_new_audio_post_request = {
                "length": snippet.length,  # length of audio in bytes
                "sample_rate_hz": snippet.sample_rate,  # sample rate
                "timestamp": timestamp_start,  # timestamp (milliseconds)
                "message_id": message_id,
                "byte_array": snippet.audio  # base64-encoded audio data
            }

            # Log the payload information
            logger.info("New audio post request prepared: length=%d, sample_rate=%d, timestamp=%d, message_id=%s",
                        snippet.length, snippet.sample_rate, timestamp_start, message_id)

            # Call the method
            try:
                self.__rest_client.on_new_audio_post(
                    on_new_audio_post_request=on_new_audio_post_request,
                    endpoint="/on_new_audio"  # Optional, as it defaults to this
                )
                logger.info("Request to /on_new_audio was successful.")
            except Exception as e:
                logger.error("An error occurred while sending the request: %s", str(e))

            return True
        except Exception as e:
            logger.error("Error in run_texttospeech_on_text: %s", str(e))
            return False
