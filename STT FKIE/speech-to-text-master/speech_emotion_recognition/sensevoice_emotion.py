# sensevoice_emotion.py

import os
import torchaudio
import torch
import io
from .Sensevoice.model import SenseVoiceSmall
from .emotion_base_model import EmotionBaseModel

TARGET_SAMPLE_RATE = 16000

class SenseVoiceEmotion(EmotionBaseModel):
    def __init__(self, model_path: str):
        self.model_path = model_path
        self.model, self.kwargs, _ = self.load_model(model_path)

    def load_model(self, model_path):
        model, kwargs = SenseVoiceSmall.from_pretrained(
            model=f"{model_path}/model/models/iic/SenseVoiceSmall", 
            device="cuda:0"
        )
        model.eval()
        return model, kwargs, None  # You can return load time if needed

    def accept_data(self, data: bytes) -> None:
        """Accepts raw audio data as bytes."""
        # waveform, sample_rate = torchaudio.load(io.BytesIO(data))
        dtype = torch.int16
        waveform = torch.frombuffer(data, dtype=dtype).float() / 32768.0  # Normalize to [-1, 1]

        # If stereo or multi-channel, reshape accordingly
        # Example: for mono (1 channel), no reshaping needed
        waveform = waveform.unsqueeze(0)  # Add channel dimension: [1, num_samples]
        
        sample_rate = 16000  
        if sample_rate != TARGET_SAMPLE_RATE:
            resampler = torchaudio.transforms.Resample(orig_freq=sample_rate, new_freq=TARGET_SAMPLE_RATE)
            waveform = resampler(waveform)
        self.waveform = waveform

    def get_results(self) -> str:
        """Returns the dominant emotion."""
        res, _ = self.model.inference(
            data_in=self.waveform,
            language="auto",
            use_itn=False,
            ban_emo_unk=True,
            **self.kwargs,
        )
        emotion_probs = dict(res[0]["possible_emotions"])
        dominant_emotion = max(emotion_probs, key=emotion_probs.get)
        return dominant_emotion

    def clear_data(self) -> None:
        """Clears internal data."""
        self.waveform = None
