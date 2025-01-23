from configobj import ConfigObj
from mqttclient import MQTTClient
from transcriber.text_to_speech import TextToSpeech


text_to_speech = TextToSpeech(config=ConfigObj('config.ini'))
chat_client = MQTTClient(
    ConfigObj('config.ini'),
    text_to_speech=text_to_speech)
chat_client.main_loop()
