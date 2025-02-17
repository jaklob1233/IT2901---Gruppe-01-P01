"""transcriber_module"""

from configobj import ConfigObj

from .transcriber_base_model import TranscriberBaseModel

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

    def __init__(self, speechtotext_variant, config: ConfigObj = None) -> None:
        print("Initializing Transcriber", flush=True)
        """
          Creates a new Transcriber object

          Parameters:
            speechtotext_variant (str): either 'Whisper' or 'Vosk'
            config (ConfigObj): Config object

          Returns:
            A new transcriber object with the given model
        """

        transcriber_config = config.get("TRANSCRIBER") #type: dict[str, ConfigObj]
        model_name = transcriber_config.get(speechtotext_variant).get("transcriber_model")

        models_config = config.get("MODELS")  #type: dict[str, ConfigObj]
        model = models_config.get(model_name)
        model_path = model.get("path")

        if speechtotext_variant == "Whisper":
            model_size = model.get("size")
            model_language = model.get("language")
            self.__transcriber = WhisperTranscriber(f"{model_size}.{model_language}", model_path)
        elif speechtotext_variant == "Vosk":
            self.__transcriber = VoskTranscriber(model_path)

        # self.models = {}
        # for model_name, model in models_config.items():
        #     transcriber = model.get("transcriber")
        #     path = model.get("path")
        #     language = model.get("language")
        #     size = model.get("size")
        #
        #     if transcriber == "vosk":
        #         self.models[model_name] = VoskTranscriber(path)
        #     elif transcriber == "whisper":
        #         self.models[model_name] = WhisperTranscriber(f"{size}.{language}", path)
        #
        # model: str = config["TRANSCRIBER"].get("transcriber", list(self.models.keys())[0])
        # self.__transcriber = self.models[model]

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
