"""transcriber_base_model"""
from abc import ABC, abstractmethod


class TranscriberBaseModel(ABC):
    """
    Abstract class defining how transcriber models should handle data

    Functions:
        accept_data (bytes) -> None:
            Passes data to chosen transcriber model

        get_results (None) -> str:
            Gets the transcribed text from the data in the chosen transcriber model

        clear_data (None) -> None:
            Voids the data stored in the transcriber

    Returns:
        A new TranscriberBaseModel object
    """
    @abstractmethod
    def accept_data(self, data: bytes) -> None:
        """
        Accepts data from a bytesstream and sends it to the transcriber model

        Parameters:
            data (bytes) bytestream to send to transcriber model.

        Returns:
            None
        """
        raise NotImplementedError()

    @abstractmethod
    def get_results(self) -> str:
        """
        Gets the result from the transcriber model

        Parameters:
            None

        Returns:
            (str) Transcribed data
        """
        raise NotImplementedError()

    @abstractmethod
    def clear_data(self) -> None:
        """
        Empties the data in the transcriber mode

        Parameters:
            None

        Return:
            None
        """
        raise NotImplementedError()
