import base64

from configobj import ConfigObj
from openapi_client.api_client import SpeechToTextApiClient
from speech_to_text.transcriber.transcriber import Transcriber
from speech_emotion_recognition.emotion_model import EmotionModel

from speech_emotion_recognition.emotion_model import EmotionModel

from pydub import AudioSegment
from pydub.silence import detect_nonsilent
import threading
import queue
import logging

logger = logging.getLogger(__name__)


class SpeechtotextConnector:
    required_sample_rate = 16000  # hertz
    required_channels = 1  # mono
    required_sample_size = 16  # bits

    silence_threshold = -10  # Silence threshold in dBFS
    silence_duration = 400  # milliseconds
    min_sentence_length = 200  # Minimum sentence length in milliseconds
    audio_buffer = None
    current_buffer_start_timestamp = None  # Tracks the timestamp of the buffer's start in ms
    collected_audio = AudioSegment.silent(duration=0)  # To collect all incoming audio

    api_client = None
    transcriber = None
    emotion_model = None 
    emotion_model = None 

    # Added attributes for queue and worker thread
    audio_queue = None
    worker_thread = None


    def initialize_speechtotext(self, speechtotext_variant, emotion_variant, transcriber_config_profile, emotion_config_profile, webhook_url):
        logger.info("Initializing speechtotext connector")
        try:
            logger.debug(f"Loading configuration from: {transcriber_config_profile}")
            transcriber_config_object = ConfigObj(transcriber_config_profile)
            emotion_config_object = ConfigObj(emotion_config_profile)

            if not transcriber_config_object:
                logger.error(f"Configuration object loaded from {transcriber_config_profile} is empty.")
                return False
            
            if not emotion_config_object:
                logger.error(f"Configuration object loaded from {emotion_config_profile} is empty.")
                return False

            if not webhook_url:
                self.api_client = None
            else:
                self.api_client = SpeechToTextApiClient(base_url=webhook_url)
            self.transcriber = Transcriber(speechtotext_variant, transcriber_config_object)
            self.emotion_model = EmotionModel(emotion_variant, emotion_config_object)
            self.audio_buffer = AudioSegment.silent(duration=0)
            self.current_buffer_start_timestamp = None

            # Stop existing worker thread **before** resetting the queue
            if self.worker_thread and self.worker_thread.is_alive():
                logger.info("Stopping existing worker thread")
                self.audio_queue.put(None)  # Send termination signal to the **existing** queue
                self.worker_thread.join(timeout=5)  # Wait for the worker thread to finish
                if self.worker_thread.is_alive():
                    logger.warning("Worker thread did not terminate in time.")

            # Now it is safe to reinitialize the queue
            self.audio_queue = queue.Queue()  # Reset the queue **after** stopping the thread
            logger.info("Starting new worker thread")
            self.worker_thread = threading.Thread(target=self.process_queue, daemon=True)
            self.worker_thread.start()

            return True
        except Exception as e:
            logger.error(f"Error in initialization: {e}")
            return False


    def run_speechtotext_on_audio(self, audio, sample_rate, length, timestamp):
        try:
            logger.info(
                f"Received audio snippet. Sample rate: {sample_rate} Hz, Length: {length} bytes, Timestamp: {timestamp} ms")

            # Step 1: Decode Base64-encoded audio to raw PCM bytes
            logger.debug("Decoding Base64-encoded audio")
            raw_audio = base64.b64decode(audio)

            # Step 2: Convert raw PCM bytes to a PyDub AudioSegment
            logger.debug("Converting raw PCM bytes to AudioSegment")
            audio_segment = AudioSegment(
                data=raw_audio,
                sample_width=2,  # 16-bit PCM = 2 bytes per sample
                frame_rate=sample_rate,
                channels=1  # Mono audio
            )

            # Step 3: Resample to 16 kHz if necessary
            if sample_rate != 16000:
                logger.debug(f"Resampling audio from {sample_rate} Hz to 16000 Hz")
                audio_segment = audio_segment.set_frame_rate(16000)

            # Collect all audio in a single buffer
            logger.debug("Adding audio snippet to collected_audio")
            self.collected_audio += audio_segment

            # Step 4: Save collected audio to a file
            output_file = "logs/collected_audio.wav"
            logger.debug(f"Exporting collected audio to {output_file}")
            self.collected_audio.export(output_file, format="wav")

            # Step 5: Add the resampled audio to the buffer
            if self.audio_buffer is None or self.audio_buffer.duration_seconds == 0:
                logger.debug("Initializing buffer start timestamp")
                self.current_buffer_start_timestamp = timestamp
                self.audio_buffer = AudioSegment.silent(duration=0)  # Initialize buffer if empty
            else:
                # Calculate the current buffer end timestamp (in milliseconds)
                current_buffer_end = self.current_buffer_start_timestamp + len(self.audio_buffer)
                # If there's a gap between the last audio and the new timestamp, insert silence.
                if timestamp > current_buffer_end:
                    gap_duration = timestamp - current_buffer_end
                    logger.debug(f"Detected gap in audio. Inserting {gap_duration} ms of silence")
                    silence_segment = AudioSegment.silent(duration=gap_duration)
                    self.audio_buffer += silence_segment

            # Append the new audio segment to the buffer
            self.audio_buffer += audio_segment
            buffer_end_timestamp = self.current_buffer_start_timestamp + len(self.audio_buffer)
            logger.debug(
                f"Appending audio segment to buffer. Current buffer timestamps: Start={self.current_buffer_start_timestamp} ms, End={buffer_end_timestamp} ms")

            # Step 6: Dynamically calculate silence threshold
            buffer_dbfs = self.audio_buffer.dBFS
            silence_thresh = max(buffer_dbfs - 15, -40)  # Dynamic threshold with a minimum of -40 dBFS
            logger.debug(f"Buffer dBFS: {buffer_dbfs:.2f}, Calculated silence threshold: {silence_thresh:.2f} dBFS")

            # Step 7: Detect nonsilent ranges
            logger.debug("Detecting nonsilent ranges in the audio buffer")
            nonsilent_ranges = detect_nonsilent(
                self.audio_buffer,
                min_silence_len=self.silence_duration,
                silence_thresh=silence_thresh
            )

            # Filter out nonsilent ranges where:
            # 1. The right border is at the end of the buffer
            # 2. The duration is less than 200 ms
            valid_nonsilent_ranges = [
                (start, end) for start, end in nonsilent_ranges
                if end < len(self.audio_buffer) and (end - start) >= self.min_sentence_length
            ]

            if not valid_nonsilent_ranges:
                logger.info("No valid nonsilent ranges detected. Waiting for more audio.")
                return True

            # Log and enqueue each valid nonsilent chunk for processing
            logger.info(f"Processing {len(valid_nonsilent_ranges)} valid nonsilent chunk(s).")
            # Assuming 'chunk' is already an AudioSegment and 'peep' is a 500 Hz sine wave beep
            for i, (start, end) in enumerate(valid_nonsilent_ranges, start=1):
                absolute_start_time = self.current_buffer_start_timestamp + start
                absolute_end_time = self.current_buffer_start_timestamp + end
                logger.info(
                    f"Enqueuing nonsilent chunk {i}: Absolute Start={absolute_start_time}ms, Absolute End={absolute_end_time}ms")

                # Extract and process chunk
                chunk = self.audio_buffer[start:end]

                # Save to file
                # output_dir = "logs/chunks"
                # os.makedirs(output_dir, exist_ok=True)
                # chunk_filename = f"chunk_{absolute_start_time}_{absolute_end_time}.wav"
                # chunk_path = os.path.join(output_dir, chunk_filename)
                # chunk.export(chunk_path, format="wav")
                # logger.info(f"Saved chunk {i} to {chunk_path}")

                # Enqueue the original chunk for further processing
                self.audio_queue.put((chunk, absolute_start_time, absolute_end_time))

            # Clear the processed portion of the buffer
            last_end = valid_nonsilent_ranges[-1][1]
            self.audio_buffer = self.audio_buffer[last_end:]
            self.current_buffer_start_timestamp += last_end

            return True
        except Exception as e:
            logger.error(f"Error processing audio: {e}")
            return False


    def process_queue(self):
        while True:
            try:
                # Log queue status before retrieving the next item
                logger.debug(f"Queue size before getting item: {self.audio_queue.qsize()}")
                item = self.audio_queue.get()

                if item is None:
                    logger.info("Worker thread received termination signal. Exiting.")
                    break

                chunk, start_time, end_time = item
                logger.info(f"Retrieved chunk from queue: Start={start_time}ms, End={end_time}ms, Chunk raw data size={len(chunk.raw_data)} bytes")

                # Process the chunk
                logger.info(f"Starting transcription for chunk: Start={start_time}ms, End={end_time}ms")
                self.transcriber.accept_data(chunk.raw_data)
                self.emotion_model.accept_data(chunk.raw_data)

                # Get results from both models
                result = self.transcriber.get_results()
                result += self.emotion_model.get_results()
                if result:
                    logger.info(f"Transcription result received: {result}")
                    print(f"Transcription result received: {result}")

                # Clear data from both models
                self.transcriber.clear_data()
                self.emotion_model.clear_data()
                logger.debug("Transcriber data cleared after processing.")

                if result:
                    if not self.api_client:
                        logger.warning("No webhook defined.")
                    else:
                        logger.info("Send transcription result to webhook.")
                        self.api_client.on_new_text_post(
                            on_new_text_post_request={
                                "timestamp_start": start_time,
                                "timestamp_end": end_time,
                                "text": result
                            },
                            _request_timeout=(5, 10)  # 5 seconds connect timeout, 10 seconds read timeout
                            # _headers={"Custom-Header": "CustomValue"}
                        )

                # Mark task as done in the queue
                self.audio_queue.task_done()

            except Exception as e:
                logger.error(f"Error processing audio chunk from queue: {e}")
                logger.debug("Error details", exc_info=True)  # Log traceback for debugging
