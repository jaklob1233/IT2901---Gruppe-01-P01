"""transcriber_module"""

from configobj import ConfigObj

from transcriber.transcriber_base_model import TranscriberBaseModel

from .vosk_transcriber import VoskTranscriber
from .whisper_transcriber import WhisperTranscriber


class Transcriber:
    """
    Transcriber class that takes in a string defining which transcription model to use.
    
    Parameters:
        config (ConfigObj): config object

    Functions:
        acceptData (bytes) -> None:
            Appends incoming data to transcribers storage
        getResults (None) -> str:
            Returns the transcribed text with audio gotten from internal storage
        clearData (None) -> None:
            Empties the internal storage of the class

    Returns:
        A VoskTranscriber object

    """
    __transcriber: TranscriberBaseModel

    def __init__(self, config: ConfigObj = None) -> None:
        """
          Creates a new Transcriber object

          Parameters:
            config (ConfigObj): Config object

          Returns:
            A new transcriber object with the given model
        """

        self.models = {}
        models_config: dict[str, ConfigObj] = config["TRANSCRIBER_MODELS"]

        for model_name, model in models_config.items():
            transcriber = model.get("transcriber")
            path = model.get("path")
            language = model.get("language")
            size = model.get("size")

            if transcriber == "vosk":
                self.models[model_name] = VoskTranscriber(path)
            elif transcriber == "whisper":
                self.models[model_name] = WhisperTranscriber(f"{size}.{language}", path)

        model: str = config["TRANSCRIBER"].get("transcriber", list(self.models.keys())[0])
        self.__transcriber = self.models[model]

    def accept_data(self, data: bytes) -> None:
        """
        Accepts data from a bytesstream and sends it to the transcriber model

        Parameters:
            data (bytes) bytestream to send to transcriber model.

        Returns:
            None
        """
        self.__transcriber.accept_data(data)

    def get_results(self) -> str:
        """
        Gets the result from the transcriber model

        Parameters:
            None

        Returns:
            (str) Transcribed data
        """
        return self.__transcriber.get_results()

    def clear_data(self) -> None:
        """
        Empties the data in the transcriber mode

        Parameters:
            None

        Return:
            None
        """
        self.__transcriber.clear_data()

    @property
    def transcriber(self) -> TranscriberBaseModel:
        """
        Gets active transcriber
        """
        return self.__transcriber
