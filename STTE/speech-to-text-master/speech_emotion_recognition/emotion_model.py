"""emotion_model.py"""

from .emotion_base_model import EmotionBaseModel
from .sensevoice_emotion import SenseVoiceEmotion
from .superb_emotion import SuperbEmotion
from configobj import ConfigObj

class EmotionModel:
    """
    Emotion model class that takes in a string defining which emotion model to use.

    This class dynamically loads and uses a specific emotion recognition model
    based on the provided configuration. Supported models include SenseVoice
    and Superb, each of which implements the EmotionBaseModel interface.
    """

    __emotion_model: EmotionBaseModel

    def __init__(self, emotion_variant: str, config: ConfigObj):
        """
        Initializes the EmotionModel by loading the appropriate backend model 
        as specified in the configuration.

        Args:
            emotion_variant (str): The name of the emotion recognizer variant 
                                as defined in the config.
            config (ConfigObj): The overall configuration object, containing 
                                model paths and settings.

        Raises:
            ValueError: If the specified model type is not recognized.
        """
        emotion_config = config.get("EMOTION_RECOGNIZER")  # type: dict[str, ConfigObj]
        model_key = emotion_config.get(emotion_variant).get("emotion_model")

        models_config = config.get("MODELS")  # type: dict[str, ConfigObj]
        model_config = models_config.get(model_key)

        model_path = model_config.get("path")  # Path to the model
        model_type = model_config.get("ser")   # "superb" or "sensevoice"

        # Choose model
        if model_type == "sensevoice":
            self.__emotion_model = SenseVoiceEmotion(model_path)
        elif model_type == "superb":
            self.__emotion_model = SuperbEmotion(model_path)
        else:
            raise ValueError(f"Unknown emotion model type: {model_type}")

    def accept_data(self, data: bytes) -> None:
        """
        Passes raw audio data to the selected emotion model for processing.

        Args:
            data (bytes): The raw audio data to process.
        """
        self.__emotion_model.accept_data(data)

    def get_results(self) -> str:
        """
        Returns the dominant emotion detected by the model.

        Returns:
            str: The name of the most likely emotion along with the confidence level.
        """
        return self.__emotion_model.get_results()

    def clear_data(self) -> None:
        """
        Clears any stored data or internal buffers in the model.
        """
        self.__emotion_model.clear_data()

    @property
    def model(self) -> EmotionBaseModel:
        """
        Exposes the internal emotion model instance.

        Returns:
            EmotionBaseModel: The currently loaded emotion model instance.
        """
        return self.__emotion_model
