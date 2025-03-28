import pyaudio
import whisper
import numpy as np
import soundfile as sf
import librosa
import io

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
            audio_stream = io.BytesIO(raw_audio)
            audio_np, sr = sf.read(audio_stream)

            # Convert to mono if stereo
            if len(audio_np.shape) > 1:
                print("[INFO] Converting stereo to mono")
                audio_np = audio_np.mean(axis=1)

            # Resample to 16000 Hz
            if sr != 16000:
                print(f"[INFO] Resampling from {sr} Hz to 16000 Hz")
                audio_np = librosa.resample(audio_np, orig_sr=sr, target_sr=16000).astype(np.float32)
                sr = 16000

            print(f"[DEBUG] Final audio shape: {audio_np.shape}, Sample rate: {sr}")

            result = self.model.transcribe(audio_np, fp16=False)
            print(f"[RESULT] Whisper output: {result}")

            return result.get("text", "")
        except Exception as e:
            print(f"[ERROR] Transcription failed: {str(e)}")
            return "[transcription error]"

    def clear_data(self) -> None:
        print(f"[CLEANUP] Clearing {len(self.data)} buffered chunks")
        self.data = []
