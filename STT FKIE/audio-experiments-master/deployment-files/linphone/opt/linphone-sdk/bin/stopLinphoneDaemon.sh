#!/bin/sh
export PATH="/opt/linphone-sdk/bin:$PATH"

SPEECHTOTEXT_IMAGE_NAME="speechtotext"
TEXTTOSPEECH_IMAGE_NAME="texttospeech"
MQTTSERVER_IMAGE_NAME="mqttserver"
MQTTTRANSPORT_IMAGE_NAME="mqtttransport"

# exit
sleep 5
linphonecsh exit

# stop docker container
docker stop ${SPEECHTOTEXT_IMAGE_NAME}
docker stop ${TEXTTOSPEECH_IMAGE_NAME}
docker stop ${MQTTSERVER_IMAGE_NAME}
docker stop ${MQTTTRANSPORT_IMAGE_NAME}
