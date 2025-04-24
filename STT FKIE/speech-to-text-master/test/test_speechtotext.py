import time
import os
import base64
import threading
import sounddevice as sd  # Use sounddevice for WSLg compatibility
import numpy as np
from scipy.io.wavfile import write, read as read_wav
from connectors.speechtotext_connector import SpeechtotextConnector
import logging

## for file test
import whisper
import torch
from pydub import AudioSegment

os.makedirs("logs", exist_ok=True)
from pydub import AudioSegment 

# Configure the log file and logger
filename = "logs/server.log"
log_dir = os.path.dirname(filename)
os.makedirs(log_dir, exist_ok=True)

logging.basicConfig(
    level=logging.DEBUG,
    format="%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s",
    force=True,
    handlers=[]
)

file_handler = logging.FileHandler(filename, mode="w")
file_handler.setLevel(logging.DEBUG)
file_formatter = logging.Formatter("%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s")
file_handler.setFormatter(file_formatter)

console_handler = logging.StreamHandler()
console_handler.setLevel(logging.WARNING)
console_formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
console_handler.setFormatter(console_formatter)

root_logger = logging.getLogger()
root_logger.addHandler(file_handler)
root_logger.addHandler(console_handler)

logger = logging.getLogger(__name__)


class TestSpeechToText:
    def __init__(self, duration=120, wav_file_path=None):
        self.duration = duration
        self.sample_rate = 16000
        self.channels = 1
        self.connector = SpeechtotextConnector()
        self.running = True
        self.wav_file_path = wav_file_path

    def capture_and_send_audio(self):
        start_time = time.time()
        buffer = []
        temp_audio = np.array([], dtype=np.int16)

        print(sd.query_devices())

        def callback(indata, frames, time_info, status):
            if status:
                logger.warning(f"Audio Status: {status}")
            buffer.append(indata.copy())

        blocksize = 1600
        try:
            with sd.InputStream(
                samplerate=self.sample_rate,
                channels=self.channels,
                dtype='int16',
                callback=callback,
                blocksize=blocksize,
                latency=0.5
            ):
                while self.running and time.time() - start_time < self.duration:
                    try:
                        if len(buffer) > 0:
                            audio_chunk = buffer.pop(0)
                            temp_audio = np.append(temp_audio, audio_chunk.flatten())

                            if len(temp_audio) >= self.sample_rate * self.channels:
                                one_second_audio = temp_audio[:self.sample_rate * self.channels]
                                temp_audio = temp_audio[self.sample_rate * self.channels:]

                                raw_audio = one_second_audio.tobytes()
                                encoded_audio = base64.b64encode(raw_audio).decode("utf-8")
                                timestamp = int((time.time() - start_time) * 1000)

                                success = self.connector.run_speechtotext_on_audio(
                                    audio=encoded_audio,
                                    sample_rate=self.sample_rate,
                                    length=len(raw_audio),
                                    timestamp=timestamp
                                )

                                if not success:
                                    logger.error(f"Error processing audio at timestamp {timestamp}ms")

                        time.sleep(0.01)
                    except Exception as e:
                        logger.error(f"Error during audio capture and processing: {e}")
                        self.running = False
        except sd.PortAudioError as e:
            logger.error(f"PortAudio error: {e}")
        except Exception as e:
            logger.error(f"General error: {e}")

    def loopback_test(self):
        samplerate = 16000
        channels = 1

        try:
            logger.info("Starting loopback test: Speak into your microphone...")

            def audio_callback(indata, outdata, frames, time_info, status):
                if status:
                    logger.info(f"Audio Status: {status}")
                outdata[:] = indata

            with sd.Stream(channels=channels, samplerate=samplerate, dtype='int16', callback=audio_callback):
                input("Press Enter to stop the loopback test...")
        except Exception as e:
            logger.error(f"Error during loopback test: {e}")

    def play_wav_file_and_send_audio(self):
        if not self.wav_file_path:
            logger.error("No WAV file path provided for 'playfile' mode.")
            return

        try:
            # Load the audio file using PyDub (this automatically handles WAV)
            audio = AudioSegment.from_wav(self.wav_file_path)

            # Normalize audio loudness
            logger.info("Normalizing audio loudness.")
            normalized_audio = audio.normalize()

            # Convert normalized audio to numpy array for further processing
            # Convert to mono if it's stereo
            if normalized_audio.channels > 1:
                normalized_audio = normalized_audio.set_channels(1)

            # Convert audio to raw bytes (PCM data)
            audio_data = np.array(normalized_audio.get_array_of_samples())
            audio_data = audio_data.astype(np.int16)  # Ensure 16-bit PCM

            # Get sample rate from the AudioSegment
            fs = normalized_audio.frame_rate

            if fs != self.sample_rate:
                logger.warning(f"Expected sample rate {self.sample_rate}, but got {fs}. Resampling may be needed.")
                return

            logger.info(f"Processing normalized audio from file: {self.wav_file_path}")
            chunk_size = self.sample_rate  # 1 second of audio

            # Process the audio in chunks of 1 second
            for i in range(0, len(audio_data), chunk_size):
                if not self.running:
                    break

                chunk = audio_data[i:i + chunk_size]
                raw_audio = chunk.astype(np.int16).tobytes()
                encoded_audio = base64.b64encode(raw_audio).decode("utf-8")

                # Simulate timestamp based on sample offset
                timestamp_ms = int((i / self.sample_rate) * 1000)

                # Call the method to process the chunk
                success = self.connector.run_speechtotext_on_audio(
                    audio=encoded_audio,
                    sample_rate=self.sample_rate,
                    length=len(raw_audio),
                    timestamp=timestamp_ms
                )

                # Sleep to simulate chunk-based processing time
                time.sleep(0.5)

                if not success:
                    logger.error(f"Error processing audio at simulated timestamp {timestamp_ms}ms")

            logger.info("Finished processing audio file, waiting 20 s")
            time.sleep(20)

        except Exception as e:
            logger.error(f"Error during WAV file playback: {e}")


    def run(self, mode="transcribe"):
        # # Load the model and processor
        #model = whisper.load_model('base.en', download_root='models/whisper_english_base')
        #
        # # Load your .wav file
        #audio_file_path = "test/samples/A_eng_f1.wav"
        #
        # # Perform the transcription
        #result = model.transcribe(audio_file_path)
        #
        # # Print the transcription
        #print("Transcription:", result['text'])

        logger.info("Starting Speech-to-Text Test...")

        if mode == "transcribe":
            initialized = self.connector.initialize_speechtotext(
                speechtotext_variant="Whisper",
                emotion_variant="SenseVoice",
                transcriber_config_profile="speech_to_text/config.ini",
                emotion_config_profile="speech_emotion_recognition/config.ini",
                webhook_url=None
            )
            if not initialized:
                logger.error("Failed to initialize the Speech-to-Text Connector.")
                return

            capture_thread = threading.Thread(target=self.capture_and_send_audio)

        elif mode == "loopback":
            capture_thread = threading.Thread(target=self.loopback_test)

        elif mode == "playfile":
            initialized = self.connector.initialize_speechtotext(
                speechtotext_variant="Whisper",
                emotion_variant="Superb",
                transcriber_config_profile="speech_to_text/config.ini",
                emotion_config_profile="speech_emotion_recognition/config.ini",
                webhook_url=None
            )
            if not initialized:
                logger.error("Failed to initialize the Speech-to-Text Connector.")
                return

            time.sleep(5)

            capture_thread = threading.Thread(target=self.play_wav_file_and_send_audio)

        else:
            logger.error("Invalid mode. Use 'transcribe', 'loopback', or 'playfile'.")
            return

        capture_thread.start()

        try:
            capture_thread.join(timeout=self.duration)
        except KeyboardInterrupt:
            print("Test interrupted by user.")
        finally:
            self.running = False
            capture_thread.join()
            print("Test completed.")


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Test Speech-to-Text or Loopback.")
    parser.add_argument("--mode", choices=["transcribe", "loopback", "playfile"], default="playfile",
                        help="Mode to run the test: 'transcribe', 'loopback', or 'playfile'.")
    parser.add_argument("--duration", type=int, default=120, help="Duration of the test in seconds.")
    parser.add_argument("--wav", type=str, help="Path to a WAV file to play in 'playfile' mode.")

    args = parser.parse_args()

    tester = TestSpeechToText(duration=args.duration, wav_file_path=args.wav)
    tester.run(mode=args.mode)
