import connexion

from werkzeug.exceptions import abort

from connectors.texttospeech_connector import TextToSpeechConnector
from openapi_server.models.provide_text_request import ProvideTextRequest  # noqa: E501

import logging
import sys
import atexit
import os

# Configure the log file and logger
filename = os.getenv("LOG_FILE", "server.log")
if not os.path.isabs(filename):
    filename = os.path.join("logs", filename)

# Set up the logger to write to the file
logging.basicConfig(
    filename=filename,
    filemode="w",
    level=logging.INFO,
    format="%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s",
    force=True,
)

logger = logging.getLogger(__name__)

# Function to flush and shut down logging
def flush_logs_on_exit():
    for handler in logger.handlers:
        handler.flush()  # Ensure all logs are written to the file
    logging.shutdown()  # Close all logging handlers

# Register the flush function to be called at program exit
atexit.register(flush_logs_on_exit)

# Custom class to redirect stdout and stderr to the logger
class StreamToLogger:
    def __init__(self, logger, level):
        self.logger = logger
        self.level = level

    def write(self, message):
        if message.strip():  # Ignore empty writes
            self.logger.log(self.level, message.strip())

    def flush(self):
        # No additional flush logic required, logging handles it
        pass

# Redirect stdout and stderr to separate log levels
sys.stdout = StreamToLogger(logger, logging.INFO)   # Redirect standard output
sys.stderr = StreamToLogger(logger, logging.ERROR)  # Redirect standard error

__texttospeech = TextToSpeechConnector()

def initialize(texttospeech_variant, config_profile, webhook_url):  # noqa: E501
    """Initialize the TextToSpeech component

    Initialize the TextToSpeech component # noqa: E501

    :type texttospeech_variant: str
    :type config_profile: str
    :type webhook_url: str
    """

    #-------------- implementation start
    success = True

    # --- speech-to-text initialization
    logger.info("Initializing texttospeech connector")
    success &= __texttospeech.initialize_texttospeech(texttospeech_variant, config_profile, webhook_url)

    if not success:
        abort(400)

    return "Initialization successfully performed!"


def on_new_audio_post(on_new_audio_post_request=None):
    # """on_new_audio_post
    #
    # :param on_new_audio_post_request: Webhook for geting part of the audio as byte array corresponding to the text provided
    # :type on_new_audio_post_request: dict | bytes
    #
    # """
    # if connexion.request.is_json:
    #     on_new_audio_post_request = OnNewAudioPostRequest.from_dict(connexion.request.get_json())  # noqa: E501
    #
    # # nothing to do here
    #
    abort(400)


def provide_text(provide_text_request=None):
    """Provide text to the TextToSpeech component

    Provide text to the TextToSpeech component # noqa: E501

    :param provide_text_request: text for the TextToSpeech component
    :type provide_text_request: dict | bytes

    """
    logger.info("Provide text request")

    if connexion.request.is_json:
        provide_text_request = ProvideTextRequest.from_dict(connexion.request.get_json())
        logger.debug(provide_text_request) # TODO remove

    # ------------------------ implementation
    success = True

    # --- call speech to text transcription
    logger.info("Running texttospeech on text")
    success = __texttospeech.run_texttospeech_on_text(provide_text_request.text,
                                                      provide_text_request.message_id,
                                                      provide_text_request.timestamp_start,
                                                      provide_text_request.timestamp_end)

    if not success:
        abort(400)
    return "Text-to-speech transformation successfully started!"
