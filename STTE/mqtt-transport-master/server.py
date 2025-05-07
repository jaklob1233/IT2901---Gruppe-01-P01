from flask import Flask, request
from apiflask.fields import String, Integer
#from flask_mqtt import Mqtt

from apiflask import APIFlask, Schema

from datetime import datetime

import json
import traceback
import logging

filename = f"logs/server-{datetime.now().strftime('%Y%m%d_%H%M%S')}.log"
logging.basicConfig(filename=filename, level=logging.INFO, format='%(asctime)s\t%(name)s\t%(levelname)s\t%(message)s')
logger = logging.getLogger(__name__)

import common_node
import paho.mqtt.client as mqtt


##

class Message(Schema):
    #destination = String()
    sender_id = String()
    receiver_id = String()
    message_id = String()
    message = String()
    timestamp = Integer()


## API
#app = Flask(__name__)

## API with docs
app = APIFlask(__name__, title="NATO IST-201 MQTT dispatcher tool", spec_path='/docs')


## MQTT
MQTT_base_topic = "NATO-IST-201/NATION-A/TEAM-ALPHA"
topic_communications = "communications"
topic_audio = "audio"
topic_broadcast = "_broadcast"
#
MQTT_VARIANT_STANDARD = 'Standard'
MQTT_VARIANT_UDP = 'MQTT-UDP'
MQTT_BROKER_URL = 'localhost'
MQTT_BROKER_PORT = 1883
#MQTT_USERNAME = 'user'
#MQTT_PASSWORD = 'secret'
MQTT_REFRESH_TIME = 1.0  # refresh time in seconds
MQTT_TLS_ENABLED = False
MQTT_CLEAN_SESSION = True
#
mqtt_client = None
MQTT_CLIENT_ID = None
#
WEBHOOK_URL = None


## MQTT
def build_topic_communications_audio(base, source_id, dest_id):
    return f"{base}/{source_id}/communications/{dest_id}/audio"

def build_topic_broadcast_audio(base, source_id):
    return f"{base}/{source_id}/communications/{topic_broadcast}/audio"


def on_connect(client, userdata, flags, rc, properties):
    logger.info(f"{client} connected with result code {rc}")
    mqtt_client.subscribe(f"{MQTT_base_topic}/+/{topic_communications}/{MQTT_CLIENT_ID}/audio")
    mqtt_client.subscribe(f"{MQTT_base_topic}/+/{topic_communications}/{topic_broadcast}/audio")


def on_message(client, userdata, message):
    logger.info(f"NEW message from topic {message.topic} message is {message.payload.decode('utf-8')}")
    # check if destination is own user: if so, send to webhook, otherwise discard
    try:
        # logger.info(f"post_to_webhook, message={json.loads(message.payload)}")
        logger.info(f"URL={WEBHOOK_URL + '/on_new_message'}")
        (result, data) = common_node.post_data(WEBHOOK_URL + "/on_new_message", None, json.loads(message.payload))
        logger.info(f"-- on_new_message POST returned {result} data={data}")
        #return 'data', result
    except Exception as e:
        logger.error(f"Exception occurred: {e}")
        logger.error(f"Stack trace:\n{''.join(traceback.format_stack())}")
        #return f"error send_message {e}", 400


def mqtt_start(client_id, server_name, port, username: str = None, password: str = None, keepalive=60):
    global mqtt_client
    mqtt_client = mqtt.Client(callback_api_version=mqtt.CallbackAPIVersion.VERSION2, client_id=client_id)
    mqtt_client.enable_logger()
    if username and password:
        mqtt_client.username_pw_set(username=username,password=password)
    mqtt_client.on_connect = on_connect
    mqtt_client.on_message = on_message
    mqtt_client.on_log = on_log
    mqtt_client.connect(server_name, port, keepalive)
    mqtt_client.loop_start()
    return mqtt_client


def on_log(client, userdata, paho_log_level, messages):
    logger.debug(f"log: {messages}")



## REST

@app.route('/mqtttransport/')
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



@app.put('/mqtttransport/initialize')
@app.input(
    {
        'mqtt_variant': String(),
        'config_profile': String(),
        'base_topic': String(),
        'mqtt_server_ip': String(),
        'mqtt_server_port': Integer(),
        'node_id': String(required=True),
        'webhook_url': String(required=True)},
    arg_name='query_data',
    location='query')
def initialize(query_data):
    global WEBHOOK_URL
    WEBHOOK_URL = query_data['webhook_url']
    global MQTT_CLIENT_ID
    MQTT_CLIENT_ID = query_data['node_id']
    if 'base_topic' in query_data:
        global MQTT_base_topic
        MQTT_base_topic = query_data['base_topic']
    if 'mqtt_server_ip' in query_data:
        global MQTT_BROKER_URL
        MQTT_BROKER_URL = query_data['mqtt_server_ip']
    if 'mqtt_server_port' in query_data:
        global MQTT_BROKER_PORT
        MQTT_BROKER_PORT = query_data['mqtt_server_port']
    global mqtt_client
    try:
        logger.info(f"initialized to node_id:{MQTT_CLIENT_ID} mqtt:{MQTT_BROKER_URL}:{MQTT_BROKER_PORT} base_topic: {MQTT_base_topic}")
        mqtt_client = mqtt_start(MQTT_CLIENT_ID, MQTT_BROKER_URL, MQTT_BROKER_PORT)
        return f"initialized to node_id:{MQTT_CLIENT_ID} mqtt:{MQTT_BROKER_URL}:{MQTT_BROKER_PORT} base_topic: {MQTT_base_topic} webhook: {WEBHOOK_URL}", 200
    except Exception as e:
        logger.error(f"Exception occurred: {e}")
        logger.error(f"Stack trace:\n{''.join(traceback.format_stack())}")
        return f"error initialising {e}", 400


@app.post('/mqtttransport/send_message')
@app.input(Message, arg_name='message')
def send_message(message=None):
    message = request.get_json(force=True)
    destination = topic_broadcast
    topic = None
    if 'receiver_id' in message:
        destination = message['receiver_id']
        topic = build_topic_communications_audio(MQTT_base_topic, MQTT_CLIENT_ID, message['receiver_id'])
    else:
        topic = build_topic_broadcast_audio(MQTT_base_topic, MQTT_CLIENT_ID)
    try:
        if mqtt_client:
            logger.info(f"SEND_MESSAGE: topic={topic} source={MQTT_CLIENT_ID} dest={destination} message={message}")
            #(rc, mid) = mqtt_client.publish(topic=topic, payload=message, qos=0, retain=False)
            (rc, mid) = mqtt_client.publish(topic=topic, payload=json.dumps(message), qos=0, retain=False)
            return f'client {MQTT_CLIENT_ID} message {message} sent', 200
        else:
            return 'error - not initialized', 400
    except Exception as e:
        logger.error(f"Exception occurred: {e}")
        logger.error(f"Stack trace:\n{''.join(traceback.format_stack())}")
        return f"error send_message {e}", 400



@app.post('/mqtttransport/send_message_dummy')
@app.input(Message, arg_name='message')
def send_message_dummy(message=None):
    logger.info(f"send_message_dummy: message={message}")
    return f'received message={message}', 200


#
#

@app.route('/mqtttransport/on_new_message', methods=['POST'])
@app.input(Message, arg_name='message')
def on_new_message():
    return "Do not use - is a dummy function", 400



## MAIN

if __name__ == '__main__':
    # setup logging
    logger.info("starting MQTT app")
    #
    #mqtt_client = mqtt_start(MQTT_CLIENT_ID, MQTT_BROKER_URL, MQTT_BROKER_PORT) #, username: str = None, password: str = None, keepalive=60)
    #app.run(debug=False)

    app.run(debug=False, port=5000)
    #
    #mqtt_client.loop_stop()
    logger.info("ending MQTT app")
