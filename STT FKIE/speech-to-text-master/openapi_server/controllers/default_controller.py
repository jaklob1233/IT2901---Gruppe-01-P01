import numpy as np
import connexion
from typing import Dict, Tuple, Union

from openapi_server.models.error import Error
from openapi_server.models.on_new_text_post_request import OnNewTextPostRequest
from openapi_server.models.provide_audio_request import ProvideAudioRequest

from speech_to_text.transcriber.whisper_transcriber import WhisperTranscriber
import base64

model_instance = None

def initialize(speechtotext_variant, config_profile, webhook_url):
    global model_instance

    if speechtotext_variant.lower() == "whisper":
        model_name = config_profile or "base"
        model_path = "./models"

        VALID_MODELS = ["tiny", "base", "small", "medium", "large"]
        if model_name not in VALID_MODELS:
            return {"error": f"Invalid model '{model_name}'. Must be one of {VALID_MODELS}"}, 400

        try:
            model_instance = WhisperTranscriber(name=model_name, path=model_path)
            return {
                "status": "Whisper initialized",
                "model": model_name,
                "webhook_url": webhook_url
            }
        except Exception as e:
            return {"error": f"Initialization failed: {str(e)}"}, 500

    return {"error": f"Unsupported variant: {speechtotext_variant}"}, 400


def provide_audio(body):
    global model_instance

    if model_instance is None:
        return {"error": "Model not initialized. Call /initialize first."}, 400

    if connexion.request.is_json:
        provide_audio_request = ProvideAudioRequest.from_dict(connexion.request.get_json())

        try:
            # Decode base64 audio bytes
            audio_bytes = base64.b64decode(provide_audio_request.byte_array)

            # Ensure even length of audio bytes
            if len(audio_bytes) % 2 != 0:
                audio_bytes = audio_bytes[:-1]

            # Convert raw audio bytes to NumPy array (PCM float32)
            audio_np = np.frombuffer(audio_bytes, dtype=np.int16).astype(np.float32)

            # Send audio to Whisper model
            model_instance.clear_data()
            model_instance.accept_data(audio_bytes)  # send raw bytes
            transcription = model_instance.get_results()
            return {"transcription": transcription}


        except Exception as e:
            return {"error": str(e)}, 500

    return {"error": "Invalid request"}, 400


def on_new_text_post(body=None):
    if connexion.request.is_json:
        on_new_text_post_request = OnNewTextPostRequest.from_dict(connexion.request.get_json())
        # Add your webhook logic here
        return {"status": "Webhook triggered"}

    return {"error": "Invalid request"}, 400
