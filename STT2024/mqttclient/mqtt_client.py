"""mqtt_client"""

import datetime
import json
import logging
import threading
import uuid
from typing import Callable
import paho.mqtt.client as mqtt
from configobj import ConfigObj
import mqttclient.database as database
import textwrap


class MQTTClient:  # pylint: disable=too-many-instance-attributes
    """
    MQTTClient
    """

    def __init__(self, config: ConfigObj = None):
        """
        Creates a new MQTTClient object

        Parameters:
            config (ConfigObj): The config object

        Returns:
            None
        """

        # Set the server address, port number, username, keepalive, and QoS
        # level
        if config and "MQTTCLIENT" in config:
            config: ConfigObj = config["MQTTCLIENT"]
            self.server_address = config.get("server_address", "0.0.0.0")
            self.port_number = int(config.get("port_number", 1883))
            self.username = config.get("username", "default username")
            self.keep_alive = int(config.get("keep_alive", 300))
            self.qos_level = int(config.get("qos_level", 1))
            if "topics" in config: 
                self.topics = config.as_list("topics")
            else:
                self.topics = []
        else:
            self.server_address = "0.0.0.0"
            self.port_number = 1883
            self.username = "default username"
            self.keep_alive = 300
            self.qos_level = 1
            self.topics = []

        # Set variables
        self.client_id = str(uuid.uuid4())
        self.database = database.Database()
        self.in_input = False
        self.message_callback = None
        self.userdata = []

        # Setup the client
        self.mqttc = mqtt.Client(
            mqtt.CallbackAPIVersion.VERSION2,
            clean_session=False,
            client_id=self.client_id)
        self.mqttc.user_data_set(self.userdata)
        self.mqttc.username_pw_set(self.username)
        self.mqttc.on_connect = self.on_connect
        self.mqttc.on_disconnect = self.on_disconnect
        self.mqttc.on_publish = self.on_publish
        self.mqttc.on_subscribe = self.on_subscribe
        self.mqttc.on_unsubscribe = self.on_unsubscribe
        self.mqttc.on_message = self.on_message

    def set_message_callback(self, callback: Callable[[str], None]):
        """
        Sets the message callback for the MQTT client

        Parameters:
            callback (function): The callback function

        Returns:
            None
        """
        self.message_callback = callback

    def on_publish(self,
                   _1,
                   userdata,
                   mid,
                   _3,
                   _4):
        """MQTT on publish callback function"""
        try:
            if mid in userdata:
                logging.info("Message ID: %s published", mid)
                userdata.remove(mid)
            else:
                logging.info("Message ID not found in userdata list")
        except Exception as e:  # pylint: disable=broad-exception-caught
            logging.error("Error: %s", e)

    def on_subscribe(
            self,
            _1,
            _2,
            _3,
            reason_code_list,
            _4):
        """MQTT on subscribe callback function"""
        if reason_code_list[0].is_failure:
            logging.warning(
                "Broker rejected you subscription: %s", reason_code_list[0])
        else:
            logging.info(
                "Broker granted the following Qos: %s",
                reason_code_list[0].value)

    def on_unsubscribe(
            self,
            _1,
            _2,
            _3,
            reason_code_list,
            _4):
        """
        MQTT Client on unsubscribe callback function
        """
        if len(reason_code_list) == 0 or not reason_code_list[0].is_failure:
            logging.info(
                "Unsubscribe succeeded (if SUBACK is received in MQTTv3 it success)")
            logging.info(
                "Unsubscribe succeeded (if SUBACK is received in MQTTv3 it success)")
        else:
            logging.error(
                "Broker replied with failure: %s", reason_code_list[0])

    def on_message(self, _1, _2, message):
        """
        MQTTClient on message callback function
        """

        topic = message.topic
        payload_in_json = json.loads(message.payload.decode())
        username = payload_in_json.get("username")
        message_text = payload_in_json.get("message")
        timestamp = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        self.database.insert_message(topic, timestamp, username, message_text)

        # If the message is from self, don't print it
        # if username == self.username:
        #    return

        # Call the message callback function if set
        if self.message_callback:
            self.message_callback(message_text)

        message_prefix = "\n" if self.in_input else ""
        logging.info(
            "%s Received message in topic %s: %s from %s",
            message_prefix, topic, message_text, username)
        print("Press L to view chatlogs...\nUse Linphone application to send message...")

    def on_connect(self, _1, _2, _3, reason_code, _4):
        """
        MQTTClient on connect callback function
        """
        if reason_code.is_failure:
            logging.error(
                "Failed to connect: %s. loop_forever() will retry connection",
                reason_code)
        else:
            logging.info("Connected")

    def on_disconnect(
            self,
            _1,
            _2,
            _3,
            reason_code,
            _4):
        """
        MQTTClient on disconnect callback function
        """
        logging.info("Disconnected with return code %s", reason_code)

    def connect(self) -> None:
        """
        Connects the MQTT client to the broker and starts the loop. The client will
        automatically subscribe to the topics that it was subscribed to before.

        Parameters:
            None

        Returns:
            None
        """
        self.mqttc.connect(
            self.server_address,
            self.port_number,
            self.keep_alive)
        self.mqttc.loop_start()
        logging.info("started mqtt client")

        # Subscribe to the topics that the client was subscribed to before
        if self.topics:
            for topic in self.topics:
                self.subscribe(topic)

    def disconnect(self) -> None:
        """
        Disconnects the MQTT client from the broker and stops the loop.


        Parameters:
            None

        Returns:
            None
        """
        self.mqttc.loop_stop()
        self.mqttc.disconnect()

    def publish(self, topic, payload) -> None:
        """
        Publishes a message to a topic

        Parameters:
            topic (str): The topic to publish the message to
            payload (str): The message to publish

        Returns:
            None
        """
        new_payload = {
            "username": self.username,
            "message": payload,
        }
        json_payload = json.dumps(new_payload)
        msg_info = self.mqttc.publish(topic, json_payload, self.qos_level)
        self.userdata.append(msg_info.mid)
        msg_info.wait_for_publish()

    def publish_to_all_topics(self, payload: str) -> None:
        """
        Publishes a message to all topics

        Parameters:
            payload (str): The message to publish

        Returns:
            None
        """
        for topic in self.topics:
            self.publish(topic, payload)
        return

    def subscribe(self, topic) -> None:
        """
        Subscribes to a topic and updates the database accordingly

        Parameters:
            topic (str): The topic to subscribe to

        Returns:
            None
        """
        logging.info(f"subscribing to {topic}")
        self.mqttc.subscribe(topic, self.qos_level)
        self.database.insert_topic(topic)
        self.database.set_subscribed(topic, 1)

    def unsubscribe(self, topic) -> None:
        """
        Unsubscribes from a topic

        Parameters:
            topic (str): The topic to unsubscribe from

        Returns:
            None
        """
        self.mqttc.unsubscribe(topic)
        self.database.set_subscribed(topic, 0)

    def print_chat_logs(self) -> None:
        """
        Prints the chatlog for all subscribed topics. Subscribed topics are retrieved from the database.

        Parameters:
            None

        Returns:
            None
        """
        for topic in self.database.get_subscribed_topics():
            topic_chat_log = self.database.get_chatlog(topic)
            if topic_chat_log:
                print("Chatlog for:", topic)
                print("=" * 92)
                print(
                    "{:<20} {:<20} {:<}".format(
                        "Timestamp",
                        "Username",
                        "Message"))
                print("-" * 92)
                for timestamp, username, message in topic_chat_log:
                    message_lines = textwrap.wrap(message, width=50)
                    message_lines = [message_lines[0]] + [" " *
                                                          42 + line for line in message_lines[1:]]

                    message = "\n".join(message_lines)
                    print(
                        "{:<20} {:<20} {:<}".format(
                            timestamp, username, message))
                print("=" * 92)
                print()
        print("Press L to view chatlogs...\nUse Linphone application to send message...")

    def main_loop(self) -> None:
        """
        Main loop for the MQTT client. The user can publish, subscribe, unsubscribe,
        or disconnect using the text based user interface. This method is not used in
        the main application, but rather used when debugging the MQTT client itself.

        Parameters:
            None

        Returns:
            None
        """
        active = True
        self.connect()
        while active:
            user_choice = input(
                "What do you want to do? (P/S/U/D/L): ").upper()
            if user_choice not in ["P", "S", "U", "D", "L"]:
                logging.info("Invalid choice")
            elif user_choice == "D":
                self.disconnect()
                active = False
            elif user_choice == "L":
                self.print_chat_logs()
            else:
                topic = input("Enter the topic: ")
                if user_choice == "P":
                    payload = input("Enter the payload: ")
                    self.publish(topic, payload)
                elif user_choice == "S":
                    self.subscribe(topic)
                elif user_choice == "U":
                    self.unsubscribe(topic)

    def threaded_func(self) -> None:
        """
        Threaded function for the main application. Consinuously listens for the users input. If input is "L" then the chatlog is printed. If input is anything else, "Invalid input" is printed.

        Parameters:
            None

        Returns:
            None
        """
        running = True
        try:
            while running:
                user_choice = input("").upper()
                if user_choice == "L":
                    self.print_chat_logs()
                else:
                    print("Invalid input")
        except KeyboardInterrupt:
            running = False
        return

    def threaded_loop(self) -> None:
        """
        Gets called in main application. Starts threaded_func() as a process.

        Parameters:
            None

        Returns:
            None
        """
        process = threading.Thread(target=self.threaded_func)
        process.start()
        return
