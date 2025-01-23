#!/bin/bash

# pulseaudio --start &
# pulseaudio -vvv
pulseaudio --start

# Load the PulseAudio null sink module
pactl load-module module-null-sink sink_name=LinphoneAudioOut sink_properties=device.description=Linphone_Audio_Out

# Execute the Docker CMD
exec "$@"
