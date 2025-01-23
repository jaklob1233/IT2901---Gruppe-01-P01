"""text_to_speech"""

import pyttsx3
from configobj import ConfigObj


class TextToSpeech:  # pylint: disable=too-few-public-methods

    """
    Class that handles the Text to Speech generation.

    Functions:
        speak (str) -> None:
            Uses the system's text-to-speech model to generate voice that
            plays back in the system's default audio output device.
    Parameters:
        configPath (str): Path to  config
    """

    def __init__(self, config: ConfigObj = None):
        self.engine = pyttsx3.init()
        if config and "TRANSCRIBER" in config:
            self.engine.setProperty(
                'rate', int(
                    config["TRANSCRIBER"].get(
                        "tts_rate", 150)))
        else:
            self.engine.setProperty('rate', 150)

    def speak(self, text):
        """
          Uses the pyttsx3 engine to speak given text parameter.

          Parameters:
            text (str) Text to be spoken.
        """
        self.engine.say(text)
        self.engine.runAndWait()
