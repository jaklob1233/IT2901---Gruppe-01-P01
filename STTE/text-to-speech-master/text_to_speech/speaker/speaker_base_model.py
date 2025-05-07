import string
from abc import ABC, abstractmethod
from connectors.text_audio_snippet import TextAudioSnippet
from typing import Optional

class SpeakerBaseModel(ABC):
    @abstractmethod
    def accept_data(self, data: string) -> None:
        raise NotImplementedError

    @abstractmethod
    def speak(self) -> Optional[TextAudioSnippet]:
        raise NotImplementedError

    @abstractmethod
    def clear_data(self) -> None:
        raise NotImplementedError