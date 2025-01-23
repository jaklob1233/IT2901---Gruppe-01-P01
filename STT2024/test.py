
from configobj import ConfigObj
from mqttclient import MQTTClient
from transcriber.text_to_speech import TextToSpeech

config = ConfigObj("config.ini")

tts = TextToSpeech(config=config)

client = MQTTClient(config=config, text_to_speech=tts)

client.main_loop()