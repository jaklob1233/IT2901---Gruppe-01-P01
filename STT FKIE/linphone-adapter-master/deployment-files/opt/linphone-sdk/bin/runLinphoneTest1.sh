#!/bin/sh

DOCKER_REGISTRY="container-registry.gitlab.cc-asp.fraunhofer.de/ist-201/images"
SPEECHTOTEXT_IMAGE_NAME="speechtotext"
SPEECHTOTEXT_VERSION="latest"
TEXTTOSPEECH_IMAGE_NAME="texttospeech"
TEXTTOSPEECH_VERSION="latest"
MQTTSERVER_IMAGE_NAME="mqttserver"
MQTTSERVER_VERSION="latest"
MQTTTRANSPORT_IMAGE_NAME="mqtttransport"
MQTTTRANSPORT_VERSION="latest"

# docker login (fhg docker registry)
echo cWfqv7yPo9boTtAaGLga| docker login -u development --password-stdin container-registry.gitlab.cc-asp.fraunhofer.de

# pull Docker images
docker pull ${DOCKER_REGISTRY}/${SPEECHTOTEXT_IMAGE_NAME}:${SPEECHTOTEXT_VERSION}
docker pull ${DOCKER_REGISTRY}/${TEXTTOSPEECH_IMAGE_NAME}:${TEXTTOSPEECH_VERSION}
docker pull ${DOCKER_REGISTRY}/${MQTTSERVER_IMAGE_NAME}:${MQTTSERVER_VERSION}
docker pull ${DOCKER_REGISTRY}/${MQTTTRANSPORT_IMAGE_NAME}:${MQTTTRANSPORT_VERSION}

# run Docker container
docker run -d --rm --network host --name ${SPEECHTOTEXT_IMAGE_NAME} \
  -e LOG_FILE="$1" \
  -v /opt/linphone-sdk/logs/speechtotext:/usr/src/app/logs \
  ${DOCKER_REGISTRY}/${SPEECHTOTEXT_IMAGE_NAME}:${SPEECHTOTEXT_VERSION} &

docker run -d --rm --network host --name ${TEXTTOSPEECH_IMAGE_NAME} \
  -e LOG_FILE="$1" \
  -v /opt/linphone-sdk/logs/texttospeech:/usr/src/app/logs \
  ${DOCKER_REGISTRY}/${TEXTTOSPEECH_IMAGE_NAME}:${TEXTTOSPEECH_VERSION} &

docker run -d --rm --network host --name ${MQTTSERVER_IMAGE_NAME} \
  -e LOG_FILE="$1" \
  -v /opt/linphone-sdk/logs/mqttserver:/mosquitto/log \
  ${DOCKER_REGISTRY}/${MQTTSERVER_IMAGE_NAME}:${MQTTSERVER_VERSION} &

docker run -d --rm --network host --name ${MQTTTRANSPORT_IMAGE_NAME} \
  -e LOG_FILE="$1" \
  -v /opt/linphone-sdk/logs/mqtttransport:/srv/mqtt-transport/logs \
  ${DOCKER_REGISTRY}/${MQTTTRANSPORT_IMAGE_NAME}:${MQTTTRANSPORT_VERSION} &

sleep 2

# init
linphonecsh init -c /home/toor/.linphonec -d 6 -l /opt/linphone-sdk/logs/linphone/linphonec.log
sleep 2

# soundcard use files: direct command instead of generic in this version!
# be aware: no "" used in command
#linphonecsh soundcard use files

sleep 3

# call
echo 'call sip:10.10.0.2'
if linphonecsh generic "call sip:10.10.0.2" | grep -q 'in progress'; then
  echo 'call in progress'
fi

# play
#echo 'play audio file'
#FILENAME=/opt/linphone-sdk/lib/lyra_testdata/A_eng_f1.wav
#linphonecsh generic "play $FILENAME"
#DURATION=$(ffprobe -i $FILENAME -show_entries format=duration -v quiet -of csv="p=0")
#OFFSET=1.0
#DURATION_PLUS=`echo $DURATION+$OFFSET | bc`
#echo "filename=$FILENAME"
#echo "sleep $DURATION_PLUS"
#sleep $DURATION_PLUS

# todo handle delay of STT initialization in filter init method
sleep 5

# wait for end of transcription of text
sleep 60

# terminate
echo 'terminate call'
linphonecsh generic terminate # [callId]

# exit
sleep 5
linphonecsh exit

# stop docker container
docker stop ${SPEECHTOTEXT_IMAGE_NAME}
docker stop ${TEXTTOSPEECH_IMAGE_NAME}
docker stop ${MQTTSERVER_IMAGE_NAME}
docker stop ${MQTTTRANSPORT_IMAGE_NAME}
