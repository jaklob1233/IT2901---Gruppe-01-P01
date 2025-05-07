import string

from text_to_speech.speaker.speaker_base_model import SpeakerBaseModel


class CoquiYourttsSpeaker(SpeakerBaseModel):
    def accept_data(self, data: string) -> None:
        pass

    def speak(self) -> bytes:
        pass

    def clear_data(self) -> None:
        pass