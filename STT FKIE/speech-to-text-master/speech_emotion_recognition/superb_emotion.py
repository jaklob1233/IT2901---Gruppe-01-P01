# speech_to_text/emotion/sensevoice_emotion.py
from .emotion_base_model import EmotionBaseModel

class SuperbEmotion(EmotionBaseModel):
    def __init__(self, model_path: str):
        self.buffer = b""
        self.model_path = model_path
        # load actual model if necessary

    def accept_data(self, data: bytes) -> None:
        self.buffer += data

    def get_results(self) -> str:
        # Dummy implementation
        return "neutral"

    def clear_data(self) -> None:
        self.buffer = b""
