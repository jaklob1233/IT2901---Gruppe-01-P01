# speech_to_text/emotion/superb_emotion.py
import os
import torch
import numpy as np

from .emotion_base_model import EmotionBaseModel
from transformers import Wav2Vec2FeatureExtractor, Wav2Vec2ForSequenceClassification

TARGET_SAMPLE_RATE = 16000

class SuperbEmotion(EmotionBaseModel):

    def __init__(self, model_path: str):
        current_dir = os.path.dirname(os.path.abspath(__file__))
        model_path = os.path.join(current_dir, "Superb")

        feature_extractor = Wav2Vec2FeatureExtractor.from_pretrained(model_path)
        model = Wav2Vec2ForSequenceClassification.from_pretrained(model_path)

        self.feature_extractor = feature_extractor
        self.model = model
        self.buffer = b""


    def accept_data(self, data: bytes) -> None:
        self.buffer += data

    def get_results(self):
        # Convert bytes buffer to numpy array of int16
        audio_array = np.frombuffer(self.buffer, dtype=np.int16)
        
        # Normalize to float32 range [-1, 1] and convert to mono
        waveform = audio_array.astype(np.float32) / 32768.0  # [4]
        
        # Process directly instead of using librosa.load
        inputs = self.feature_extractor(
            waveform, 
            sampling_rate=16000,
            return_tensors="pt",
            padding=True
        )

        with torch.no_grad():
            logits = self.model(**inputs).logits

        probs = torch.nn.functional.softmax(logits, dim=-1)
        id2label = self.model.config.id2label

        emotion_probs = {id2label[i].capitalize(): float(probs[0][i]) for i in range(len(id2label))}
        predicted_emotion = max(emotion_probs, key=emotion_probs.get)
        probability = emotion_probs[predicted_emotion]
        
        self.clear_data()
        
        return f' ({predicted_emotion}, {probability:.2f})'

    def clear_data(self) -> None:
        self.buffer = b""
