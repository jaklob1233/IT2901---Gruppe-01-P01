# FFI-STT

This project is an automatic Speech-to-text application that runs on radio systems to minimize bandwidth usage by sending text instead of audio data.

## User Manual

This is a simple user manual on how to install, run and interact with An automated transcription service. It is built on Linphone, mqtt, vosk and whisper using Python and C++.

### Installation

The installation is split into 4 parts: **Cloning the repository**, **Ubuntu Package Installation**, **Model Download**, **Python Package Installation and Linphone Compilation**. 

Here are the steps to install the application:

1. Cloning the repository gets you access to the code. The code can be found [here](https://github.com/hermahs/FFI-STT). 
2. Navigate to the directory.
3. Install the necessary Ubuntu packages with `sudo bash install_packages.sh`
    - For this step you need sudo (*super user do*) access
4. Download the models by running `./install_models.sh`
5. Install python libraries, compile the linphone source code and setup pulse audio with `./install.sh`

After these steps have been completed, the application is installed. To see how to configure it and run it, see **How to run** and **Config file**

### How to run

Here are the basics with how to run the application:
1. Adjust the configuration found in `config.ini`
2. Run the application with `python3 main.py`

The application will then start. If the application does not start you are returned to the terminal. You will know the application have started successfully if the text *Press L to view chatlogs...* and *Use Linphone application to send message* appear on the screen.

### How to interact

The way you interact with the application is through a VoIP client like [Linphone](https://www.linphone.org/). You simply call the ip address of the system with the username as *module*, so the sip address will look something like this: `module@x.x.x.x` where `x.x.x.x` is the systems running the applications ip.

## Config file

The main config file for the application is found in `config.ini`. You can also specify your own custom config file by using the `CONFIGPATH` envrionment variable.

The config file is structured with nested sections. A normal section looks like this: `[section]` and a nested section looks like this: `[[nested-section]]`. Please be sure to correctly configure the different values and sections to have the configuration work.

### Config file structure

There are 4 main parts of the config file:
* **MQTT Client**
* **Transcriber**
* **Linphone**
* **Linphone Core**
Each of these has their own sections with different config options.

#### MQTT Client

The MQTT Client config are related to the MQTT Client including which topics are subscribed to.

* username: username of client | defaults to `default username`
* server_address: address of MQTT Broker | defaults to `0.0.0.0`
* port_number: port of MQTT Broker | defaults to `1883`
* keep_alive: Keep connection alive | defaults to `False`
* qos_level: Quality of service level | defaults to 1
* topics: List of topics | defaults to empty list

#### Transcriber

The Transcriber config are related to the Transcriber and Text-To-Speech.

* transcriber: active transcriber | defaults to `vosk`. Options are `vosk` or `whisper` 
* tts_rate: text to speech rate | defaults to `150`

#### Linphone

The linphone config includes the configuration of the audio processing done on an active call

* rate: audio frequency | defaults to 44100
* chunk_size: size of audio buffer | defaults to 1024

#### Linphone Core

The Linphone Core config handles the Linphone Core Config. Please see [this page](https://archive.flossmanuals.net/linphone/ch009_configuring.html) for full overview of all possible options.

## How to setup in a virtual machine

The setup process for the application to run on a virtual machine is as follows:
1. Pull the repostiory
    - If you pull it onto your local machine you need to copy it over to the VM
    - If you pull it directly onto the VM, you don't need to do anything more
2. Run `sudo bash install_packages.sh` as sudo to install packages
3. Run `install.sh` to install python libraries needed, STT-models, compiles linphone python library, and setups pulseaudio
4. Run the application with `python3 main.py`. The python file is located in the root directory of the repository.

### Things to note
* Sometimes the application is a little buggy, and will not connect to the MQTT broker. If this happens, just restart the application. 
* You need the whole repo as it is intended to be used as a singular application. This means that you only need to run 1 python file for the whole application to work. 
* To call into the application, you only need the machine's IP address. The application will automatically bind to the address.
