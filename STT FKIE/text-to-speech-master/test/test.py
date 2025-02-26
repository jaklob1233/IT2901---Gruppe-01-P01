from TTS.utils.synthesizer import Synthesizer
from pydub import AudioSegment
from pydub.playback import play
import time
import logging
import sys
import atexit
import torch

# Configure the log file and logger
filename = "logs/server.log"
output_path = "logs/output.wav"  # Define the original output path
resampled_output_path = "logs/output_16khz.wav"

# Set up the logger to write to the file
logging.basicConfig(
    filename=filename,
    filemode="w",
    level=logging.DEBUG,
    format="%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s",
    force=True,
)

logger = logging.getLogger(__name__)

# Function to flush and shut down logging
def flush_logs_on_exit():
    for handler in logger.handlers:
        handler.flush()  # Ensure all logs are written to the file
    logging.shutdown()  # Close all logging handlers

# Register the flush function to be called at program exit
atexit.register(flush_logs_on_exit)

# Custom class to redirect stdout and stderr to the logger
class StreamToLogger:
    def __init__(self, logger, level):
        self.logger = logger
        self.level = level

    def write(self, message):
        if message.strip():  # Ignore empty writes
            self.logger.log(self.level, message.strip())

    def flush(self):
        # No additional flush logic required, logging handles it
        pass

# Redirect stdout and stderr to separate log levels
sys.stdout = StreamToLogger(logger, logging.INFO)   # Redirect standard output
sys.stderr = StreamToLogger(logger, logging.ERROR)  # Redirect standard error

model_path = "models/tacotron2-DCA/model_file.pth"
config_path = "models/tacotron2-DCA/config_local.json"

# Check if CUDA is available
if torch.cuda.is_available():
    logger.info("CUDA is available! PyTorch can use the GPU.")
    logger.info(f"Number of GPUs: {torch.cuda.device_count()}")
    logger.info(f"GPU Name: {torch.cuda.get_device_name(0)}")
else:
    logger.info("CUDA is not available. PyTorch is running on the CPU.")

# Initialize synthesizer
synthesizer = Synthesizer(
    tts_checkpoint=model_path,
    tts_config_path=config_path,
    use_cuda=False,
)

# Get user input for text-to-speech
text = input("Enter text to synthesize: ")

# Log start time
start_time = time.time()

# Synthesize text
logger.info("Starting text-to-speech synthesis.")
waveform = synthesizer.tts(text)

# Log end of synthesis
end_synthesis_time = time.time()
logger.info(f"Text-to-speech synthesis completed in {end_synthesis_time - start_time:.2f} seconds.")

# Save the synthesized audio
synthesizer.save_wav(waveform, output_path)
logger.info(f"Audio saved to: {output_path}")

# Resample the audio to PCM 16-bit at 16 kHz using pydub
try:
    audio = AudioSegment.from_file(output_path)
    resampled_audio = (
        audio.set_frame_rate(16000)  # Resample to 16 kHz
        .set_sample_width(2)        # 16-bit audio
        .set_channels(1)            # Mono
    )
    resampled_audio.export(resampled_output_path, format="wav")
    logger.info(f"Resampled audio saved to: {resampled_output_path}")
except Exception as e:
    logger.info(f"Error during resampling: {e}")

# Play the resampled audio
try:
    logger.info("Playing the resampled audio...")
    audio_to_play = AudioSegment.from_file(resampled_output_path)
    play(audio_to_play)
except Exception as e:
    logger.info(f"Error playing resampled audio: {e}")
