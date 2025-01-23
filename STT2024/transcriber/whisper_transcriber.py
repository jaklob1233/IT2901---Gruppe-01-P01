"""whisper_transcriber"""

import logging
import tempfile
import wave

import pyaudio
import whisper

from transcriber.transcriber_base_model import TranscriberBaseModel


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

    def get_results(self) -> str:
        with tempfile.NamedTemporaryFile(delete=True, suffix=".wav") as t:
            wf = wave.open(t.name, 'wb')
            wf.setnchannels(1)
            wf.setsampwidth(self.p.get_sample_size(pyaudio.paInt16))
            wf.setframerate(44100)
            wf.writeframes(b''.join(self.data))
            wf.close()
            logging.info(t.name)
            result = self.model.transcribe(t.name)["text"]

        return result

    def clear_data(self) -> None:
        self.data = []
