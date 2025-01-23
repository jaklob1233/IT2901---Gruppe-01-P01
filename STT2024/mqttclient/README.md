# MQTT Client

Custom MQTT Client with logging of messages created using Paho Client and SQLite3.

## Installation
To build the library from a single line, you can run ./install.sh in the source directory which will install all the needed dependecies, compile the code, and copy the resulting binary to the correct directories.

## MQTTClient Module

### Constructor

#### MQTTClient (config: ConfigObj, TextToSpeech: TextToSpeech):
Initializes a new MQTTClient object.

##### Parameters:

config: ConfigObj: Config object containing the values set in the config
(default is None).

##### Properties:

userdata: A list containing the message ids for published messages.

config_object: Holds the configuration read from the specified config file.

server_address: Server address from the config file.

port_number: Port number from the config file.

username: Username from the config file.

KEEPALIVE: Max seconds to wait before pinging the MQTT broker.

QOS_LEVEL: Quality of Service level for the MQTT messages.

topics: The topics that should be subscribed to.

inInput: Boolean flag for active input.

database: Database.py object used to read from and write to SQLite3 database containing chatlog

client_id: Randomly generated client ID.

message_callback: A function to be called if the client recieves a message.

### Functions:

#### set_message_callback(self, callback: Callable[[str], None]):
Sets the callback function for the MQTT client.

#### on_publish(client, userdata, mid, reason_code, properties):
Callback function invoked when a message is published.
Handles publishing events.

#### on_subscribe(client, userdata, mid, reason_code_list, properties):
Callback function invoked when a subscription is made.
Handles subscription events.

#### on_unsubscribe(client, userdata, mid, reason_code_list, properties):
Callback function invoked when a subscription is cancelled.
Handles unsubscription events.

#### on_message(client, userdata, message):
Callback function invoked when a message is received.
Handles incoming messages and inserts them into the database.

#### on_connect(client, userdata, flags, reason_code, properties):
Callback function invoked when the client connects to the MQTT broker.
Handles connection events.

#### on_disconnect(client, userdata, disconnect_flags, reason_code, properties):
Callback function invoked when the client disconnects from the MQTT broker.
Handles disconnection events.

#### connect():
Establishes a connection to the MQTT broker and starts the client loop.

#### disconnect():
Stops the client loop and disconnects from the MQTT broker.

#### publish(topic, payload):
Publishes a message to the specified topic.
##### Parameters:
topic (str): The topic to publish to.

payload (str): The message payload.

#### publish_to_all_topics(payload: str)
Publish a message to all topics.
##### Parameters:
payload (str): The message payload.

#### subscribe(topic):
Subscribes to the specified topic.
##### Parameters:
topic (str): The topic to subscribe to.

#### unsubscribe(topic):
Unsubscribes from the specified topic.
##### Parameters:
topic (str): The topic to unsubscribe from.

#### print_chat_logs():
Prints the chalog for each subscribed topic.

#### main_loop():
Main loop for the MQTT client, providing a text-based user interface.
Allows the user to publish, subscribe, unsubscribe, or disconnect.

#### threaded_func():
Function run as process in main application to listen for input.

#### threaded_loop():
Function called in main application that starts threaded_func() as a process.

## Database Module

### Constructor

#### Database():
Initializes a new Database object.

##### Parameters:
None

##### Properties:
None

### Functions

#### create_connection():
Creates a connection to the SQLite database.
Returns: Connection to the SQLite database or None if there is an error.

#### create_tables(conn):
Creates necessary tables in the SQLite database.
##### Parameters:
conn (sqlite3.Connection): Connection to the SQLite database.

#### insert_message(topic, timestamp, username, message):
Inserts a message into the SQLite database.
##### Parameters:
topic (str): The topic of the message.

timestamp (str): The timestamp of the message.

username (str): The username of the sender.

message (str): The message text.

#### insert_topic(topic):
Inserts a topic into the SQLite database.
##### Parameters:
topic: (str): The topic inserted into the database.

#### get_chatlog(topic):
Retrieves the last 10 messages from the chat log for a given topic.
##### Parameters:
topic (str): The topic to retrieve the chat log for.
##### Returns:
List of tuples containing the timestamp, username, and message text.

#### get_subscribed_topics()
Gets all the topics with subscribed set to true.
##### Returns:
List of strings corresponding to topic names.

#### set_subscribed(topic, subscribed):
Sets the subscribed field in topic table
##### Parameters:
topic (str): The topic to update.

subscribed (bool): The value to set the subscribed field to.

## Configuration
The MQTT client configuration is stored in the config.ini file in the source directory. Modify the configuration file in the "MQTTCLIENT" section to set up the MQTT broker connection parameters. These are
 - username
 - server_address
 - port_number
 - keep_alive
 - qos_level
 - topics


## Usage
Run the script MQTTClient.py to start the MQTT client. The client provides a command-line interface for interacting with the MQTT broker:

- P: Publish a message to a topic.
- S: Subscribe to a topic.
- U: Unsubscribe from a topic.
- D: Disconnect from the MQTT broker and exit.

## Acknowledgments
[Paho MQTT](https://eclipse.dev/paho/index.php?page=clients/python/index.php)\
[SQLite3](https://docs.python.org/3/library/sqlite3.html#reference)