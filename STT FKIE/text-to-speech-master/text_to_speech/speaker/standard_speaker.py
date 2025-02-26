import base64
import logging
import os
import time
from pydub import AudioSegment
from TTS.utils.synthesizer import Synthesizer
from typing import Optional

from connectors.text_audio_snippet import TextAudioSnippet
from text_to_speech.speaker.speaker_base_model import SpeakerBaseModel

logger = logging.getLogger(__name__)


def ensure_punctuation(text: str) -> str:
    if text.strip()[-1] not in {".", "!", "?"}:
        text = text.strip() + "."
    return text


class CoquiSpeaker(SpeakerBaseModel):
    """
    Class that handles Text-to-Speech generation using Coqui TTS.
    """

    __synthesizer: Synthesizer
    __data: str

    __LOGS_FILE = "/usr/src/app/logs/tmp.wav"
    __RESAMPLED_FILE = "/usr/src/app/logs/tmp_resampled.wav"

    def __init__(self, model_path: str, config_path: str):
        """
        Initialize the Coqui TTS synthesizer.

        :param model_path: Path to the model file (e.g., model_file.pth).
        :param config_path: Path to the configuration file (e.g., config.json).
        """
        self.__synthesizer = Synthesizer(
            tts_checkpoint=model_path,
            tts_config_path=config_path
        )
        self.__data = ""

    def accept_data(self, data: str) -> None:
        """
        Append text data to be synthesized.

        :param data: Text to append.
        """
        self.__data += data

    def speak(self) -> Optional[TextAudioSnippet]:
        """
        Generate speech audio using Coqui TTS.

        :return: TextAudioSnippet containing the synthesized text and audio.
        """
        if not self.__data:
            logger.warning("No data provided for text-to-speech. Skipping audio generation.")
            return None

        logger.info("Starting text-to-speech process with Coqui TTS.")

        try:
            self.__data = ensure_punctuation(self.__data)
            # Generate waveform
            logger.debug("Generating waveform for text: %s", self.__data)

            # Log start time
            start_time = time.time()

            # Synthesize text
            print("Starting text-to-speech synthesis.")
            waveform = self.__synthesizer.tts(self.__data)

            # Log end of synthesis
            end_synthesis_time = time.time()
            print(f"Text-to-speech synthesis completed in {end_synthesis_time - start_time:.2f} seconds.")

            # Log the current working directory
            logger.debug(f"Logs path: {self.__LOGS_FILE}")

            # Save the original audio to a temporary file
            logger.debug("Saving original audio to temporary file: %s", self.__LOGS_FILE)
            try:
                # Ensure the target file is truncated
                with open(self.__LOGS_FILE, 'wb') as f:
                    pass  # This will clear the file

                # Save the waveform
                self.__synthesizer.save_wav(waveform, self.__LOGS_FILE)
                logger.info(f"File successfully saved at {self.__LOGS_FILE}")
            except Exception as e:
                logger.error(f"Failed to save WAV file: {e}")

            # Resample audio to 16 kHz PCM 16-bit mono using pydub
            logger.debug("Resampling audio to 16 kHz PCM 16-bit mono.")
            audio = AudioSegment.from_file(self.__LOGS_FILE)
            resampled_audio = (
                audio.set_frame_rate(16000)  # 16 kHz
                .set_sample_width(2)  # 16-bit PCM
                .set_channels(1)  # Mono
            )
            resampled_audio.export(self.__RESAMPLED_FILE, format="wav")

            # Encode the audio directly into Base64
            logger.debug("Encoding resampled audio to Base64.")
            with open(self.__RESAMPLED_FILE, "rb") as audio_file:
                base64_encoded_audio = base64.b64encode(audio_file.read()).decode('utf-8')

            # Create a TextAudioSnippet
            text_audio_snippet = TextAudioSnippet(
                self.__data,
                base64_encoded_audio,
                len(resampled_audio.raw_data),
                resampled_audio.frame_rate
            )

            logger.info("Text-to-speech process completed successfully.")
            return text_audio_snippet

        except Exception as e:
            logger.error("An error occurred during the text-to-speech process: %s", str(e))
            return None

    def clear_data(self) -> None:
        """
        Clear the stored text data and remove temporary files.
        """
        self.__data = ''
        # if os.path.exists(self.__LOGS_FILE):
        #     os.remove(self.__LOGS_FILE)
        # if os.path.exists(self.__RESAMPLED_FILE):
        #     os.remove(self.__RESAMPLED_FILE)
