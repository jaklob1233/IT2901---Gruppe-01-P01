"""whisper_transcriber"""

import pyaudio
import whisper
import numpy as np

from speech_to_text.transcriber.transcriber_base_model import TranscriberBaseModel

import logging
logger = logging.getLogger(__name__)


class WhisperTranscriber(TranscriberBaseModel):
    """
    Creates a WhisperTranscriber class.

    Parameters:
        name (str): name of model file
        path (str): path to model location

    Functions:
        accept_data (bytes) -> None:
            Appends incoming data to transcribers storage
        get_results (None) -> str:
            Returns the transcribed text with audio gotten from internal storage
        clear_data (None) -> None:
            Empties the internal storage of the class

        Returns:
            A WhisperTranscriber object
    """

    def __init__(self, name: str, path: str) -> None:
        self.model = whisper.load_model(name, download_root=path)
        self.p = pyaudio.PyAudio()
        self.data = []

    def accept_data(self, data: bytes) -> None:
        self.data.append(data)

    def get_results(self):
        # Convert byte data to numpy array (16-bit PCM → float32)
        audio_np = np.frombuffer(b''.join(self.data), dtype=np.int16).astype(np.float32)

        # Normalize PCM 16-bit audio to range [-1, 1] (Whisper requirement)
        audio_np /= 32768.0

        # Transcribe using Whisper
        whisper_result = self.model.transcribe(audio_np)

        return whisper_result["text"]


    def clear_data(self) -> None:
        self.data = []
