# User Manual

This is a simple user manual on how to install, run and interact with An automated transcription service. It is built on Linphone, mqtt, vosk and whisper using Python and C++.

## Installation

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