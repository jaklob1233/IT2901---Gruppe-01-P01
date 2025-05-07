"""emotion_base_model.py"""
from abc import ABC, abstractmethod

class EmotionBaseModel(ABC):
    """
    Abstract class defining how emotion recognition models should handle data

    Functions:
        accept_data (bytes) -> None:
            Passes audio data to the chosen emotion model

        get_results (None) -> str:
            Gets the detected emotion from the stored data

        clear_data (None) -> None:
            Empties the internal buffer
    """

    @abstractmethod
    def accept_data(self, data: bytes) -> None:
        raise NotImplementedError()

    @abstractmethod
    def get_results(self) -> str:
        raise NotImplementedError()

    @abstractmethod
    def clear_data(self) -> None:
        raise NotImplementedError()
