import pyaudio
import whisper
import numpy as np

from speech_to_text.transcriber.transcriber_base_model import TranscriberBaseModel


class WhisperTranscriber(TranscriberBaseModel):
    def __init__(self, name: str, path: str) -> None:
        print(f"[INIT] Loading Whisper model: {name} from {path}")
        self.model = whisper.load_model(name, download_root=path)
        self.p = pyaudio.PyAudio()
        self.data = []

    def accept_data(self, data: bytes) -> None:
        print(f"[DATA] Received audio chunk of size: {len(data)} bytes")
        self.data.append(data)

    def get_results(self) -> str:
        raw_audio = b''.join(self.data)
        print(f"[DEBUG] Total raw audio size: {len(raw_audio)} bytes")

        try:
            # Skip WAV header (first 44 bytes)
            if raw_audio[:4] == b'RIFF':
                print("[INFO] Detected WAV header — stripping 44 bytes")
                raw_audio = raw_audio[44:]

            # Convert to float32 PCM
            audio_np = np.frombuffer(raw_audio, dtype=np.int16).astype(np.float32)
            audio_np /= 32768.0  # Normalize

            print(f"[DEBUG] Audio shape: {audio_np.shape}, dtype: {audio_np.dtype}")

            # Transcribe
            result = self.model.transcribe(audio_np, fp16=False)
            print(f"[RESULT] Whisper output: {result}")

            return result.get("text", "")
        except Exception as e:
            print(f"[ERROR] Transcription failed: {str(e)}")
            return "[transcription error]"

    def clear_data(self) -> None:
        print(f"[CLEANUP] Clearing {len(self.data)} buffered chunks")
        self.data = []
