import requests
import base64

url = 'http://localhost:8080/speechtotext/'
audio_file = 'test-sound/test.wav'

init_params = {
    "speechtotext_variant": "Whisper",  #
    "config_profile": "speech_to_text/config.ini",
    "webhook_url": "http://localhost:5000/webhook"
}

initialize = requests.put(url+"initialize", params=init_params)

if initialize.status_code == 200:
    print("Initialization successful:", initialize.json())
else:
    print(f"Failed to initialize. Status code: {initialize.status_code}, Response: {initialize.text}")

with open(audio_file, 'rb') as audio_file:
    byte_array = base64.b64encode(audio_file.read()).decode('utf-8')
    
payload = {
    "byte_array": byte_array,
    "length": len(byte_array),
    "sample_rate_hz": 48000,
    "timestamp": 0 
}

headers = {
    "Content-Type": "application/json",
    "accept": "*/*"
}

response = requests.post(url + "provide_audio", json=payload, headers=headers)


print(response.json())