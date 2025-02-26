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
#docker pull ${DOCKER_REGISTRY}/${MQTTSERVER_IMAGE_NAME}:${MQTTSERVER_VERSION}
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

#docker run -d --rm --network host --name ${MQTTSERVER_IMAGE_NAME} \
#  -e LOG_FILE="$1" \
#  -v /opt/linphone-sdk/logs/mqttserver:/mosquitto/log \
#  ${DOCKER_REGISTRY}/${MQTTSERVER_IMAGE_NAME}:${MQTTSERVER_VERSION} &

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
#linphonecsh generic "mute"

# wait for incoming call
echo 'wating for incoming call...'
INCOMING=FALSE
while [ $INCOMING = FALSE ]
do
  if linphonecsh generic calls | grep -q 'IncomingReceived'; then
    INCOMING=TRUE
  fi

  sleep 0.1
done

# answer
echo 'answer incoming call'
if linphonecsh generic answer | grep -q 'Media streams established'; then
  echo 'media streams established'
fi

# record
#rm -f /opt/linphone-sdk/record.wav
#linphonecsh generic "record /opt/linphone-sdk/record.wav"

# wait for end of call
echo 'wating for end of call...'
END_OF_CALL=FALSE
while [ $END_OF_CALL = FALSE ]
do
  if linphonecsh generic calls | grep -q 'No active call'; then
    END_OF_CALL=TRUE
  fi

  sleep 0.1
done

echo 'call ended'

# exit
sleep 5
linphonecsh exit

# stop docker container
docker stop ${SPEECHTOTEXT_IMAGE_NAME}
docker stop ${TEXTTOSPEECH_IMAGE_NAME}
#docker stop ${MQTTSERVER_IMAGE_NAME}
docker stop ${MQTTTRANSPORT_IMAGE_NAME}
