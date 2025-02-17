"""vosk_transcriber"""

import json

from vosk import Model, KaldiRecognizer

from .transcriber_base_model import TranscriberBaseModel

class VoskTranscriber(TranscriberBaseModel):
    """
    Creates a VoskTranscriber class.

    Parameters:
        path (str): Path to vosk model

    Functions:
        acceptData (bytes) -> None:
            Appends incoming data to transcribers storage
        getResults (None) -> str:
            Returns the transcribed text with audio gotten from internal storage
        clearData (None) -> None:
            Empties the internal storage of the class

    Returns:
        A VoskTranscriber object
    """

    def __init__(self, path: str) -> None:
        self.model = Model(path)

        self.rec = KaldiRecognizer(self.model, 16000)
        self.rec.SetWords(True)
        self.rec.SetPartialWords(True)

    def accept_data(self, data) -> None:
        self.rec.AcceptWaveform(data)

    def get_results(self) -> str:
        return ''.join(json.loads(self.rec.FinalResult())['text'])

    def clear_data(self) -> None:
        self.rec.Reset()
