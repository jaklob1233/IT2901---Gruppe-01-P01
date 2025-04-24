# speech_to_text/emotion/superb_emotion.py
import os
import torch
import base64
import librosa
from transformers import Wav2Vec2FeatureExtractor, Wav2Vec2ForSequenceClassification

from .emotion_base_model import EmotionBaseModel

class SuperbEmotion(EmotionBaseModel):

    def __init__(self, model_path: str):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        model_path = os.path.join(current_dir, "Superb")

        feature_extractor = Wav2Vec2FeatureExtractor.from_pretrained(model_path)
        model = Wav2Vec2ForSequenceClassification.from_pretrained(model_path)

        self.buffer = b""
        self.feature_extractor = feature_extractor
        self.model = model


    def accept_data(self, data: bytes) -> None:
        decoded_data = base64.b64decode(data)
        self.buffer += decoded_data

    def get_results(self) -> str:
        y, sr = librosa.load(self.buffer, sr=16000, mono=True)
        inputs = self.feature_extractor(y, sampling_rate=16000, return_tensors="pt", padding=True)

        with torch.no_grad():
            logits = self.model(**inputs).logits

        probs = torch.nn.functional.softmax(logits, dim=-1)
        id2label = self.model.config.id2label

        # Ensure correct key formatting
        emotion_probs = {id2label[i].capitalize(): float(probs[0][i]) for i in range(len(id2label))}
        predicted_emotion = max(emotion_probs, key=emotion_probs.get)

        return predicted_emotion, emotion_probs

    def clear_data(self) -> None:
        self.buffer = b""
