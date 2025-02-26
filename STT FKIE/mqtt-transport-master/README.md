# MQTT Transport Server

This code implements a MQTT API server to exchange messages over MQTT.
It is implemented in flask due to its simplicity.
Due to specificities of the MQTT client, **do NOT run in debug mode**.

It follows the API specification from NATO IST-201:
- initialize:  setup of the server
- send_message:  send message over MQTT
- on_new_message: callback function indicating that a new message is received

See `mqtttransport.yaml` for OpenAPI specification.


## Install

run `pip install -r requirements.txt`

Needs a MQTT server to be installed.  


## Start the server

run `flask --app server-py run`


## Test

### generate message (nodeB->nodeA)

> mosquitto_pub -t NATO-IST-201/NATION-A/TEAM-ALPHA/nodeB/communications/nodeA/audio -m '{"message": "hello good people", "message_id": "1", "receiver_id": "nodeA", "sender_id": "nodeB", "timestamp": 1732202565379}'

### generate message (nodeB->broadcast)

> mosquitto_pub -t NATO-IST-201/NATION-A/TEAM-ALPHA/nodeB/communications/_broadcast/audio -m '{"message": "hello good people", "message_id": "1", "sender_id": "nodeB", "timestamp": 1732202565379}'


## Documentation

> flask --app server.py spec --output api.json



