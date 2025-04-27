import connexion

from werkzeug.exceptions import abort
from openapi_server.models.provide_audio_request import ProvideAudioRequest
from connectors.speechtotext_connector import SpeechtotextConnector

import logging
import sys
import atexit
import os

speechtotext = SpeechtotextConnector()

# Configure the log file and logger
filename = os.getenv("LOG_FILE", "server.log")
if not os.path.isabs(filename):
    filename = os.path.join("logs", filename)

# Set up the logger to write to the file
logging.basicConfig(
    filename=filename,
    filemode="w",
    level=logging.DEBUG,
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


def initialize(speechtotext_variant, emotion_variant, transcriber_config_profile, emotion_config_profile, webhook_url):
    """Initialize the SpeechToText component
    :type speechtotext_variant: str
    :type emotion_variant: str
    :type transcriber_config_profile: str
    :type emotion_config_profile: str
    :type webhook_url: str

    :rtype: Union[None, Tuple[None, int], Tuple[None, int, Dict[str, str]]
    """

    #-------------- implementation start
    success = True

    # --- speech-to-text initialization
    logger.info("Initializing speechtotext connector")
    success &= speechtotext.initialize_speechtotext(speechtotext_variant=speechtotext_variant, emotion_variant=emotion_variant,
                                                    transcriber_config_profile=transcriber_config_profile, emotion_config_profile=emotion_config_profile,
                                                    webhook_url=webhook_url)
    if not success:
        abort(400)

    return "Initialization successfully performed!"



# ---- not needed
def on_new_text_post(on_new_text_post_request=None):
#     """on_new_text_post
#
#
#     :param on_new_text_post_request: Webhook for delivering new text from speech recognition to client
#     :type on_new_text_post_request: dict | bytes
#
#     :rtype: Union[None, Tuple[None, int], Tuple[None, int, Dict[str, str]]
#     """
#
#
#     if connexion.request.is_json:
#         on_new_text_post_request = OnNewTextPostRequest.from_dict(connexion.request.get_json())  # noqa: E501
#
#     # nothing to do here
#
    abort(400)


def provide_audio(provide_audio_request=None):
    """Provide audio to the SpeechToText component

    :param provide_audio_request: part of the audio (e.g. 20 ms) as byte array
    :type provide_audio_request: dict | bytes

    :rtype: Union[None, Tuple[None, int], Tuple[None, int, Dict[str, str]]
    """
    logger.info("Provide audio request")

    if connexion.request.is_json:
        provide_audio_request = ProvideAudioRequest.from_dict(connexion.request.get_json())  # noqa: E501
        logger.debug(provide_audio_request)

    # --- call speech to text transcription
    logger.info("Running speechtotext on audio")
    success = speechtotext.run_speechtotext_on_audio(audio=provide_audio_request.byte_array,
                                                     sample_rate=provide_audio_request.sample_rate_hz,
                                                     length=provide_audio_request.length,
                                                     timestamp=provide_audio_request.timestamp)

    if not success:
        abort(400)
    return "Speech-to-text transcription successfully started!"