# Speech Emotion Recognition

This project implements speech emotion recognition (SER) using SenseVoice and Superb.

## Requirements

- Python 3.8 or newer
- Docker

## Install and Run (Locally)

1. **Clone the repository** (or unzip if you have the folder):

   ```bash
   cd speech_emotion_recognition
   ```

2. **Create and activate a virtual environment (recommended):**

   ```bash
   python3 -m venv venv
   source venv/bin/activate  # On Windows: venv\Scripts\activate
   ```

3. **Install dependencies:**

   ```bash
   pip install -r requirements.txt
   ```

4. **Download models (required for first-time use):**

   ```bash
   python downloadModels.py
   ```

---

## Run with Docker

1. **Build the Docker image:**

   ```bash
   docker build -t speech_emotion_recognition .
   ```

2. **Run the container:**
   ```bash
   docker run --rm -it speech_emotion_recognition
   ```

---

## Testing the models
   The models can be tested in ```speech-to-text-master/test/test_speechtotext.py```
   - remember to configure which model by changing ```emotion_variant= ``` to either ```Sensevoice``` or ```Superb``` in ``` def run(self, mode="playfile")```
   

## Notes

- Downloading models require internet access.
- You might want to mount a volume when using Docker if you want to keep downloaded models persistent.
