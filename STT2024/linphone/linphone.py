"""linphone"""

import threading
import time
import logging
import math
import os
import sys
import pyaudio
from configobj import ConfigObj
from mqttclient.mqtt_client import MQTTClient
from transcriber import transcriber_base_model
from transcriber.text_to_speech import TextToSpeech
from .linphone_module import Core  # pylint: disable=no-name-in-module
# import wave


def clear_terminal():
    """
    Clears the terminal.

    Parameters:
        None

    Returns:
        None
    """
    os.system('cls' if os.name == 'nt' else 'clear')


def find_pulse_audio(l: list[str]):
    """
    Finds pulse audio unit in list of all audio sources

    Parameters:
        l (list[str]): list of audio sources

    Returns:
        index (int): Index of pulse audio if found, otherwise -1
    """
    for index, item in enumerate(l):
        if "Linphone_Audio_Out" in item:
            return index
    return -1


def json_to_xml_config(config: dict[str, str | int]) -> str:
    """
    Translates json config to xml config that Linphone can use

    Parameters:
        config (dict[str, str | int]): json config object

    Returns:
        xml_string (str): json config as xml string
    """

    config_string = """<?xml version="1.0" encoding="UTF-8"?>
        <config xmlns="http://www.linphone.org/xsds/lpconfig.xsd"
        xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
        xsi:schemaLocation="http://www.linphone.org/xsds/lpconfig.xsd lpconfig.xsd">\n"""

    for section, section_dict in config.items():
        section_config_string = f"\t<section name=\"{section}\">\n"

        for key, value in section_dict.items():
            key_config_string = f"\t\t<entry name=\"{key}\">{value}</entry>\n"
            section_config_string += key_config_string
        section_config_string += "\t</section>\n"

        config_string += section_config_string

    config_string += "</config>"

    return config_string


class Linphone:  # pylint: disable=too-many-instance-attributes
    """
    The Linphone Class handles everything to do with Linphone, from incoming calls
    to audio processing those calls.

    Parameters:
        transcriber (TranscriberBaseModel): Transcriber model
        mqtt (MQTTClient): MQTT client
        config (ConfigObj): config object

    Functions:
        run (None) -> None:
            The run function starts the Linphone applications and listens for
            incoming calls and handles them when they come in.
    """
    core: Core
    record_process: threading.Thread

    def __init__(
            self,
            transcriber: transcriber_base_model.TranscriberBaseModel = None,
            mqtt: MQTTClient = None,
            text_to_speech: TextToSpeech = None,
            config: ConfigObj = None) -> None:
        """
        Creates a new Linphone object

        Parameters:
            transcriber (TranscriberBaseModel): Transcriber model
            mqtt (MQTTClient): MQTT client
            configPath (str): Path to linphone config

        Returns:
            A new Linphone object
        """
        self.audio = pyaudio.PyAudio()
        self.format = pyaudio.paInt16
        self.channels = 1
        self.frames = []
        self.record = False
        self.record_process = None
        self.transcriber = transcriber
        if mqtt:
            self.mqtt = mqtt
            self.mqtt.set_message_callback(self.handle_mqtt_message)
        self.text_to_speech = text_to_speech
        self.result = None
        self.pulse_device = 0

        for i in range(self.audio.get_device_count()):
            d = self.audio.get_device_info_by_index(i)
            if d["name"] == "pulse":
                self.pulse_device = i

        self.core = Core()

        if config and "LINPHONE" in config:
            self.chunk = int(config["LINPHONE"].get("chunk_size", 1024))
            self.rate = int(config["LINPHONE"].get("rate", 44100))
        else:
            self.chunk = 1024
            self.rate = 44100

        if config and "LINPHONE_CORE" in config:
            self.core.loadConfigFromXML(
                json_to_xml_config(
                    config["LINPHONE_CORE"]))
            # we need to force the setting of the sipAddress for some reason
            # it does not automatically load it from the config
            self.core.sipAddress = config["LINPHONE_CORE"]["sip"].get(
                "contact", "sip:module@0.0.0.0")
        else:
            self.core.sipAddress = "sip:module@0.0.0.0"

    def handle_record(self) -> None:
        """handle record"""
        stream: pyaudio.Stream = self.audio.open(
            format=self.format,
            channels=self.channels,
            rate=self.rate,
            input=True,
            input_device_index=0,
            frames_per_buffer=self.chunk
        )
        stream.start_stream()
        start_time = time.time()
        while self.record:
            time.sleep(0.05)
        end_time = time.time()
        for _ in range(0, math.ceil(
                (self.rate / self.chunk) * (end_time - start_time))):
            data = stream.read(self.chunk)
            self.frames.append(data)
            self.transcriber.accept_data(data)
        stream.stop_stream()

    def handle_call_callback(self, state) -> None:
        """handle call callback"""
        # create match cases here
        match state:
            case "End":
                # end recording
                self.record = False
                if self.record_process is not None:
                    self.record_process.join()
                # Include in order to listen to captures audio
                # wf = wave.open('test.wav', 'wb')
                # wf.setnchannels(self.channels)
                # wf.setsampwidth(self.audio.get_sample_size(self.format))
                # wf.setframerate(self.rate)
                # wf.writeframes(b''.join(self.frames))
                # wf.close()

                # handle data
                if self.transcriber is not None:
                    self.result = self.transcriber.get_results()
                    logging.info("Got message: %s", self.result)
                else:
                    self.frames = []

                if self.mqtt is not None and self.result is not None:
                    self.mqtt.publish_to_all_topics(self.result)

                self.result = None
                logging.info("State End")
            case "Released":
                if self.transcriber is not None:
                    self.transcriber.clear_data()
                print(
                    "Press L to view chatlogs...\nUse Linphone application to send message...")

            case "Connected":
                # start recording
                self.record_process = threading.Thread(
                    target=self.handle_record)
                self.record = True
                self.record_process.start()
        logging.info("callback result: %s", state)

    def handle_mqtt_message(self, message: str) -> None:
        """
        Receive messages from MQTT and pass them to TextToSpeech module

        Parameters:
            message (str): The message received from MQTT

        Returns:
            None
        """
        if self.text_to_speech is not None:
            self.text_to_speech.speak(message)

    def run(self) -> None:
        """
        Starts the Linphone service. This will listen for incoming calls and handle them
        until it receives a keyboard interrupt or the application crashes.

        Parameters:
            None

        Returns:
            None
        """
        try:
            self.mqtt.connect()
            self.mqtt.threaded_loop()

            source = self.core.soundSources[find_pulse_audio(
                self.core.soundSources)]
            self.core.setSoundSource(source)

            start = self.core.start()
            if not start:
                logging.critical("COULD NOT START CORE")
                return

            self.core.run()
            logging.debug("Core on: %s", self.core.on)
            logging.info(
                "Service has started - address: %s",
                self.core.sipAddress)
            
            time.sleep(0.1)
            clear_terminal()
            print(
                "Press L to view chatlogs...\nUse Linphone application to send message...")

            while True:
                if self.core.hasActiveCalls:
                    call = self.core.getCall()
                    call.setCallback(self.handle_call_callback)
                    call.acceptCall()
                time.sleep(0.1)

        except KeyboardInterrupt:
            logging.info("we are exiting")
            self.core.stopRun()
            self.core.stop()
            sys.exit(1)

        except Exception as e:  # pylint: disable=broad-exception-caught
            logging.critical("BREAKING ERROR!")
            logging.critical(e)
            self.core.stopRun()
            self.core.stop()
            sys.exit(1)
