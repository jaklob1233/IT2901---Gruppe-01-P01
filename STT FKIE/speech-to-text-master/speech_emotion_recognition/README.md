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

5. **Run the desired script:**

   For SenseVoice-based emotion recognition:

   ```bash
   python sensevoice_emotion.py
   ```

   For Superb-based emotion recognition:

   ```bash
   python superb_emotion.py
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

## Notes

- Downloading models require internet access.
- You might want to mount a volume when using Docker if you want to keep downloaded models persistent.
