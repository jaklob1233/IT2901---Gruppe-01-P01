# speech_to_text/emotion/emotion_model.py
from .emotion_base_model import EmotionBaseModel
from .sensevoice_emotion import SenseVoiceEmotion
# from .superb_emotion import SuperbEmotion
from configobj import ConfigObj

class EmotionModel:
    __emotion_model: EmotionBaseModel

    def __init__(self, emotion_variant: str, config: ConfigObj):
        # model_config = config.get("EMOTION")
        # model_name = model_config.get(emotion_variant).get("emotion_model")
        # models_config = config.get("MODELS")
        # model_path = models_config.get(model_name).get("path")

        # if emotion_variant == "SenseVoice":
        #     self.__emotion_model = SenseVoiceEmotion(model_path)
        # elif emotion_variant == "Superb":
        #     self.__emotion_model = SuperbEmotion(model_path)
        # else:
        #     raise ValueError(f"Unknown emotion model: {emotion_variant}")
        emotion_config = config.get("EMOTION_RECOGNIZER")  # type: dict[str, ConfigObj]
        model_key = emotion_config.get(emotion_variant).get("emotion_model")

        models_config = config.get("MODELS")  # type: dict[str, ConfigObj]
        model_config = models_config.get(model_key)

        model_path = model_config.get("path")
        model_type = model_config.get("ser")  # "superb" or "sensevoice"

        if model_type == "sensevoice":
            self.__emotion_model = SenseVoiceEmotion(model_path)
        elif model_type == "superb":
            self.__emotion_model = SuperbEmotion(model_path)
        else:
            raise ValueError(f"Unknown emotion model type: {model_type}")

    def accept_data(self, data: bytes) -> None:
        self.__emotion_model.accept_data(data)

    def get_results(self) -> str:
        return self.__emotion_model.get_results()

    def clear_data(self) -> None:
        self.__emotion_model.clear_data()

    @property
    def model(self) -> EmotionBaseModel:
        return self.__emotion_model
