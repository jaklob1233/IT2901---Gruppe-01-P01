#!/usr/bin/env bash
# install python libraries
python3 -m venv env

source env/bin/activate

# install torch before to avoid killing the whole install
pip3 install torch --no-cache-dir

pip3 install -r requirements.txt

# compile the linphone library
(cd linphone; cmake -B build ; cmake --build build)
linphone_module_full=$(find linphone/build -type f -name "*.so")
IFS='/' read -ra NAMES <<< $linphone_module_full
linphone_module_name=${NAMES[-1]}
cp $linphone_module_full ./$linphone_module_name
cp $linphone_module_full ./linphone/$linphone_module_name

[ -d ~/.local/share/linphone ] || mkdir -p ~/.local/share/linphone

# fix pulse
pulseaudio --start

pactl load-module module-null-sink sink_name=LinphoneAudioOut sink_properties=device.description=Linphone_Audio_Out

pactl unload-module module-suspend-on-idle

# Create user?

# setup script as a service to start automatically?
