import os
import whisper
from glob import glob

# Load the Whisper model
model = whisper.load_model("large-v2")

# Path to the folder containing your WAV files
audio_folder = '/home/toor/projects/IST-201/audio-experiments/corpora/itu/'  # Change this to your folder path

# Get a list of all WAV files in the folder (recursively)
audio_files = glob(os.path.join(audio_folder, '*.wav'))

# Loop through each audio file
for audio_file in audio_files:
    # Perform speech-to-text transcription using Whisper
    print(f"Transcribing {audio_file}...")
    result = model.transcribe(audio_file)

    # Prepare the output text file name (same name as the audio file)
    output_text_file = os.path.splitext(audio_file)[0] + '.txt'

    # Write the transcription to the text file
    with open(output_text_file, 'w') as f:
        f.write(result['text'])

    print(f"Transcription saved to {output_text_file}")

print("Annotation complete for all audio files.")
