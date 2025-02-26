from configobj import ConfigObj

from text_to_speech.speaker.coqui_xtts_speaker import CoquiXttsSpeaker
from text_to_speech.speaker.coqui_yourtts_speaker import CoquiYourttsSpeaker
from text_to_speech.speaker.standard_speaker import CoquiSpeaker
from text_to_speech.speaker.speaker_base_model import SpeakerBaseModel

import logging
import os

logger = logging.getLogger(__name__)


class Speaker:
    __speaker: SpeakerBaseModel

    __STANDARD = "standard"
    __COQUI_YOURTTS = "coqui_yourtts"
    __COQUI_XTTS = "coqui_xtts"

    def __init__(self, texttospeech_variant, config: ConfigObj = None) -> None:
        logger.info("Initializing Speaker")

        model_name = texttospeech_variant
        if config and "MODELS" in config:
            model_name = config["MODELS"].get(texttospeech_variant).get("name")
            logger.debug("Model name determined from config: %s", model_name)

        speaker_config = None
        if config and "SPEAKER" in config:
            speaker_config = config.get("SPEAKER")
            logger.debug("Speaker configuration loaded: %s", speaker_config)

        # initialize the speaker
        match texttospeech_variant:
            case self.__STANDARD:
                # Define the paths for the model and config files
                model_path = "models/tacotron2-DCA/model_file.pth"
                config_path = "models/tacotron2-DCA/config.json"

                logger.info("Initializing standard speaker variant")
                self.__speaker = CoquiSpeaker(model_path,
                                              config_path)
            case self.__COQUI_YOURTTS:
                logger.info("Initializing Coqui YourTTS speaker variant")
                self.__speaker = CoquiYourttsSpeaker()
            case self.__COQUI_XTTS:
                logger.info("Initializing Coqui Bark speaker variant")
                self.__speaker = CoquiXttsSpeaker()
            case _:
                logger.error("Unknown texttospeech variant: %s", texttospeech_variant)
                raise ValueError("Unknown texttospeech variant")

    def accept_data(self, data):
        self.__speaker.accept_data(data)

    def speak(self):
        return self.__speaker.speak()

    def clear_data(self):
        self.__speaker.clear_data()

    @property
    def speaker(self) -> SpeakerBaseModel:
        return self.__speaker
