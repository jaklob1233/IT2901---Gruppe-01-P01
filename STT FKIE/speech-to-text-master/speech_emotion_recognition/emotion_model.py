# speech_to_text/emotion/emotion_model.py
from .emotion_base_model import EmotionBaseModel
from .sensevoice_emotion import SenseVoiceEmotion
# from .superb_emotion import SuperbEmotion
from configobj import ConfigObj

class EmotionModel:
    __emotion_model: EmotionBaseModel

    def __init__(self, emotion_variant: str, config: ConfigObj):
    #     model_config = config.get("EMOTION")
    #     model_name = model_config.get(emotion_variant).get("emotion_model")
    #     models_config = config.get("MODELS")
    #     model_path = models_config.get(model_name).get("path")

    #     if emotion_variant == "SenseVoice":
    #         self.__emotion_model = SenseVoiceEmotion(model_path)
    #     elif emotion_variant == "Superb":
    #         self.__emotion_model = SuperbEmotion(model_path)
    #     else:
    #         raise ValueError(f"Unknown emotion model: {emotion_variant}")
        pass

    def accept_data(self, data: bytes) -> None:
        self.__emotion_model.accept_data(data)

    def get_results(self) -> str:
        return self.__emotion_model.get_results()

    def clear_data(self) -> None:
        self.__emotion_model.clear_data()

    @property
    def model(self) -> EmotionBaseModel:
        return self.__emotion_model
