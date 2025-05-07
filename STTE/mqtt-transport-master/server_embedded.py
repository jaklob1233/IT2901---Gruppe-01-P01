from flask import Flask, request
from flask_mqtt import Mqtt

from apiflask import APIFlask

## API
#app = Flask(__name__)

## API with docs
app = APIFlask(__name__, title="NATO IST-201 MQTT dispatcher tool", spec_path='/api/docs')



## MQTT
base_topic = "NATO-IST-201/chat"
broadcast_topic = "broadcast"
#
app.config['MQTT_BROKER_URL'] = 'localhost'
app.config['MQTT_BROKER_PORT'] = 1883
#app.config['MQTT_USERNAME'] = 'user'
#app.config['MQTT_PASSWORD'] = 'secret'
app.config['MQTT_REFRESH_TIME'] = 1.0  # refresh time in seconds
app.config['MQTT_TLS_ENABLED'] = False
app.config['MQTT_CLEAN_SESSION'] = True
app.config['MQTT_CLIENT_ID'] = "NATO-IST201-chat-user-1"
#
mqtt = Mqtt(app)


#mqtt.unsubscribe('home/mytopic')
#mqtt.unsubscribe_all()
#mqtt.publish('home/mytopic', 'this is my message')

@mqtt.on_connect()
def handle_connect(client, userdata, flags, rc):
    print("MQTT connect")
    mqtt.subscribe("%s/#" % (base_topic))

@mqtt.on_message()
def handle_mqtt_message(client, userdata, message):
    print(f"MQTT message: \n- topic {message.topic} \n- content: {message.payload.decode()}")
    data = dict(
        topic=message.topic,
        payload=message.payload.decode()
    )


##

@app.route('/')
def index():
    return "API for NATO-IST 201"

# @app.route('/message')
# def message():
#     if request.method == 'POST':
#         print("POST message")
#         pass
#     elif request.method == 'GET':
#         print("GET message")
#         pass
#     return "general message"


@app.route('/send_message/<source>', methods=['POST'])
def send_message_broadcast(source=None):
    destination = broadcast_topic
    topic = f"{base_topic}/{destination}"
    message = request.get_json(force=True)
    #
    if request.method == 'POST':
        print(f"POST from {source} to {destination}: message")
        pass
    elif request.method == 'GET':
        print(f"GET from {source} to {destination}: message")
        pass
    return f"SEND_MESSAGE BROADCAST: topic {topic} source {source} dest {destination} message {message}"


@app.route('/send_message/<source>/<destination>', methods=['POST'])
def send_message(source=None, destination=None):
    if not destination:
        destination = broadcast_topic
    topic = f"{base_topic}/{destination}"
    message = request.get_json(force=True)
    #
    if request.method == 'POST':
        print(f"POST from {source} to {destination}: message")
        pass
    elif request.method == 'GET':
        print(f"GET from {source} to {destination}: message")
        pass
    return f"SEND_MESSAGE: topic {topic} source {source} dest {destination} message {message}"


@app.route('/on_new_message/<source>/<destination>', methods=['POST'])
def on_new_message_DO_NOT_USE_EXAMPLE_ONLY(source=None, destination=None):
    if not destination:
        destination = broadcast_topic
    message = request.get_json(force=True)
    #
    if request.method == 'POST':
        print(f"POST from {source} to {destination}: message")
        pass
    elif request.method == 'GET':
        print(f"GET from {source} to {destination}: message")
        pass
    return f"NEW MESSAGE: source {source} dest {destination} message"


## MAIN

if __name__ == '__main__':
    #
    app.run(debug=False)
    #mqtt_client.loop_stop()
