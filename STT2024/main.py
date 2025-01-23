"""Application"""

import logging
from os import environ
from configobj import ConfigObj
from linphone.linphone import Linphone
from mqttclient import MQTTClient

from transcriber.transcriber import Transcriber

from transcriber.text_to_speech import TextToSpeech

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s')


class Application():  # pylint: disable=too-few-public-methods
    """
    Class for main application

    Parameters:
        config_path (str): path to config file

    Functions:
        run (None) -> None:
            starts the application
    """

    def __init__(self, config_path: str) -> None:
        self.config = ConfigObj(config_path)

    def run(self) -> None:
        """
        Starts the application

        Parameters:
            None

        Returns:
            None
        """

        mqtt = MQTTClient(config=self.config)

        transcriber = Transcriber(config=self.config)

        text_to_speech = TextToSpeech(config=self.config)

        linphone = Linphone(
            transcriber=transcriber,
            mqtt=mqtt,
            text_to_speech=text_to_speech,
            config=self.config)
        linphone.run()


if __name__ == "__main__":
    if "CONFIGPATH" in environ:
        CONFIG_PATH = environ["CONFIGPATH"]
    else:
        CONFIG_PATH = "config.ini"

    app = Application(CONFIG_PATH)
    app.run()
