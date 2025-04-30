"""sensevoice_emotion.py"""

import torchaudio
import torch
from .Sensevoice.model import SenseVoiceSmall
from .emotion_base_model import EmotionBaseModel

class SenseVoiceEmotion(EmotionBaseModel):
    """
    Wrapper class for the SenseVoiceSmall emotion recognition model.

    This class loads a pretrained SenseVoiceSmall model, processes incoming audio data,
    and runs inference to determine the most likely expressed emotion.

    Attributes:
        model_path (str): Path to the model directory.
        model (SenseVoiceSmall): Loaded model instance.
        kwargs (dict): Additional keyword arguments needed for inference.
        waveform (torch.Tensor): Audio data ready for inference.
    """

    def __init__(self, model_path: str):
        """
        Initializes the model from the specified path.

        Args:
            model_path (str): The path to the root directory containing the pretrained model.
        """
        self.model_path = model_path
        self.model, self.kwargs = self.load_model(model_path)

    def load_model(self, model_path):
        """
        Loads the SenseVoiceSmall model from the given directory.

        Args:
            model_path (str): Path to the model folder.

        Returns:
            tuple: The loaded model and its inference kwargs.
        """
        model, kwargs = SenseVoiceSmall.from_pretrained(
            model=f"{model_path}/model/models/iic/SenseVoiceSmall", 
            device="cuda:0"
        )
        model.eval()
        return model, kwargs

    def accept_data(self, data: bytes) -> None:
        """
        Accepts raw audio data as bytes and processes it into a waveform tensor.

        The audio is assumed to be 16-bit PCM, mono, and sampled at 16kHz.

        Args:
            data (bytes): The raw audio data to process.
        """
        dtype = torch.int16 # Specify the data type for 16-bit PCM
        waveform = torch.frombuffer(data, dtype=dtype).float() / 32768.0  # Normalize to [-1, 1]
        waveform = waveform.unsqueeze(0)  # Add single (mono) channel dimension: [1, num_samples] 
        self.waveform = waveform 

    def get_results(self) -> str:
        """
        Performs inference and returns the most likely emotion and its probability.

        Returns:
            str: The dominant emotion and its probability formatted as 'emotion (prob)'.
        """
        res, _ = self.model.inference(
            data_in=self.waveform,
            language="auto",
            use_itn=False,
            ban_emo_unk=True,
            **self.kwargs,
        )
        emotion_probs = dict(res[0]["possible_emotions"])
        dominant_emotion = max(emotion_probs, key=emotion_probs.get)
        probability = emotion_probs[dominant_emotion]
        return f" ({dominant_emotion}, {probability:.2f})"

    def clear_data(self) -> None:
        """
        Clears any stored waveform data to free up memory and prepare for the next inference.
        """
        self.waveform = None
