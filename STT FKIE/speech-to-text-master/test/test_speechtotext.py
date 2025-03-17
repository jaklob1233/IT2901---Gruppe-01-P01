import time
import base64
import threading
import sounddevice as sd  # Use sounddevice for WSLg compatibility
import numpy as np
from scipy.io.wavfile import write
from connectors.speechtotext_connector import SpeechtotextConnector
import logging

# Configure the log file and logger
filename = "logs/server.log"

# Set up the root logger to write to both the file and the console
logging.basicConfig(
    level=logging.DEBUG,  # Root logger's logging level
    format="%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s",
    force=True,
    handlers=[]  # Prevent basicConfig from adding handlers
)

# Add file handler
file_handler = logging.FileHandler(filename, mode="w")
file_handler.setLevel(logging.DEBUG)
file_formatter = logging.Formatter("%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s")
file_handler.setFormatter(file_formatter)

# Add console handler
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.WARNING)
console_formatter = logging.Formatter("%(asctime)s - %(levelname)s - %(message)s")
console_handler.setFormatter(console_formatter)

# Get the root logger and add the handlers
root_logger = logging.getLogger()
root_logger.addHandler(file_handler)
root_logger.addHandler(console_handler)

logger = logging.getLogger(__name__)


class TestSpeechToText:
    def __init__(self, duration=120):
        self.duration = duration  # Duration of test in seconds
        self.sample_rate = 16000
        self.channels = 1
        self.connector = SpeechtotextConnector()
        self.running = True

    def capture_and_send_audio(self):
        start_time = time.time()
        buffer = []  # Buffer to store incoming audio
        temp_audio = np.array([], dtype=np.int16)  # Temporary buffer for 1-second audio chunks

        print(sd.query_devices())

        def callback(indata, frames, time_info, status):
            if status:
                logger.warning(f"Audio Status: {status}")
            buffer.append(indata.copy())

        # Start audio stream using sounddevice with a fixed blocksize
        blocksize = 1600  # 0.1 seconds of audio (1600 samples at 16 kHz)
        try:

            with sd.InputStream(
                    samplerate=self.sample_rate,
                    channels=self.channels,
                    dtype='int16',
                    callback=callback,
                    blocksize=blocksize,
                    latency=0.5  # 500 ms latency
            ):
                while self.running and time.time() - start_time < self.duration:
                    try:
                        if len(buffer) > 0:
                            # Append new audio to the temporary buffer
                            audio_chunk = buffer.pop(0)
                            temp_audio = np.append(temp_audio, audio_chunk.flatten())

                            # Check if the temporary buffer contains 1 second of audio
                            if len(temp_audio) >= self.sample_rate * self.channels:
                                # Extract 1 second of audio and reset the temporary buffer
                                one_second_audio = temp_audio[:self.sample_rate * self.channels]
                                temp_audio = temp_audio[self.sample_rate * self.channels:]

                                # Encode the 1-second audio as Base64
                                raw_audio = one_second_audio.tobytes()
                                encoded_audio = base64.b64encode(raw_audio).decode("utf-8")

                                # Calculate the timestamp
                                timestamp = int((time.time() - start_time) * 1000)  # Timestamp in milliseconds

                                # Send the audio chunk to the connector
                                success = self.connector.run_speechtotext_on_audio(
                                    audio=encoded_audio,
                                    sample_rate=self.sample_rate,
                                    length=len(raw_audio),  # Pass the correct byte length (should be 32000 bytes)
                                    timestamp=timestamp
                                )

                                if not success:
                                    logger.error(f"Error processing audio at timestamp {timestamp}ms")

                        # Sleep briefly to avoid CPU overuse
                        time.sleep(0.01)  # Reduced sleep for faster accumulation
                    except Exception as e:
                        logger.error(f"Error during audio capture and processing: {e}")
                        self.running = False
        except sd.PortAudioError as e:
            logger.error(f"PortAudio error: {e}")
        except Exception as e:
            logger.error(f"General error: {e}")


    def loopback_test(self):
        samplerate = 16000  # Sample rate in Hz
        channels = 1  # Mono

        try:
            logger.info("Starting loopback test: Speak into your microphone...")

            def audio_callback(indata, outdata, frames, time_info, status):
                if status:
                    logger.info(f"Audio Status: {status}")
                outdata[:] = indata  # Copy input audio to output for loopback

            with sd.Stream(channels=channels, samplerate=samplerate, dtype='int16', callback=audio_callback):
                input("Press Enter to stop the loopback test...")
        except Exception as e:
            logger.error(f"Error during loopback test: {e}")


    def run(self, mode="transcribe"):
        logger.info("Starting Speech-to-Text Test...")

        if mode == "transcribe":
            # Initialize the connector
            initialized = self.connector.initialize_speechtotext(
                speechtotext_variant="Whisper",
                config_profile="speech_to_text/config.ini",
                webhook_url="http://localhost:5000/webhook"
            )

            if not initialized:
                logger.error("Failed to initialize the Speech-to-Text Connector.")
                return

            # Start the audio capture and processing thread
            capture_thread = threading.Thread(target=self.capture_and_send_audio)
        elif mode == "loopback":
            # Start the audio capture and playback thread
            capture_thread = threading.Thread(target=self.loopback_test)
        else:
            logger.error("Invalid mode. Use 'transcribe' or 'loopback'.")
            return

        capture_thread.start()

        try:
            # Wait for the test duration or user interruption
            capture_thread.join(timeout=self.duration)
        except KeyboardInterrupt:
            print("Test interrupted by user.")
        finally:
            self.running = False
            capture_thread.join()
            print("Test completed.")

# Run the test class if executed directly
if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser(description="Test Speech-to-Text or Loopback.")
    parser.add_argument("--mode", choices=["transcribe", "loopback"], default="transcribe",
                        help="Mode to run the test: 'transcribe' for transcription or 'loopback' for testing the microphone.")
    parser.add_argument("--duration", type=int, default=120, help="Duration of the test in seconds.")

    args = parser.parse_args()

    tester = TestSpeechToText(duration=args.duration)
    tester.run(mode=args.mode)
