#!/bin/sh
export PATH="/opt/linphone-sdk/bin:$PATH"

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
docker run -d --rm --network host --name ${SPEECHTOTEXT_IMAGE_NAME} -v /opt/linphone-sdk/logs/speechtotext:/usr/src/app/logs ${DOCKER_REGISTRY}/${SPEECHTOTEXT_IMAGE_NAME}:${SPEECHTOTEXT_VERSION} &
docker run -d --rm --network host --name ${TEXTTOSPEECH_IMAGE_NAME} -v /opt/linphone-sdk/logs/texttospeech:/usr/src/app/logs ${DOCKER_REGISTRY}/${TEXTTOSPEECH_IMAGE_NAME}:${TEXTTOSPEECH_VERSION} &
docker run -d --rm --network host --name ${MQTTSERVER_IMAGE_NAME} -v /opt/linphone-sdk/logs/mqttserver:/mosquitto/log ${DOCKER_REGISTRY}/${MQTTSERVER_IMAGE_NAME}:${MQTTSERVER_VERSION} &
docker run -d --rm --network host --name ${MQTTTRANSPORT_IMAGE_NAME} -v /opt/linphone-sdk/logs/mqtttransport:/srv/mqtt-transport/logs ${DOCKER_REGISTRY}/${MQTTTRANSPORT_IMAGE_NAME}:${MQTTTRANSPORT_VERSION} &

# init
linphonecsh init -c /home/toor/.linphonec -d 6 -l $1
sleep 2

# soundcard use files: direct command instead of generic in this version!
# be aware: no "" used in command
#linphonecsh soundcard use files
