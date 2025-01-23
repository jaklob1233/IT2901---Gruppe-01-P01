#!/usr/bin/env bash

apt-get update -y && apt-get upgrade -y
apt-get install -y python3.10 python3-pip python3.10-venv pulseaudio liblinphone-dev sqlite3 dbus libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0 g++ cmake unzip espeak pybind11-dev
apt-get clean

chmod +x install_models.sh