#!/bin/sh

sudo rm gen/* -rf

# --------------
# SPEECHTOTEXT
# --------------
# C
docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/speechtotext.yaml \
  -g c \
  -o /local/gen/c/speechtotext/client

docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/speechtotext.yaml \
  -g cpp-pistache-server \
  -o /local/gen/c/speechtotext/server

# Python
docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/speechtotext.yaml \
  -g python \
  -o /local/gen/python/speechtotext/client

docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/speechtotext.yaml \
  -g python-flask \
  -o /local/gen/python/speechtotext/server

# --------------
# TEXTTOSPEECH
# --------------
# C
docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/texttospeech.yaml \
  -g c \
  -o /local/gen/c/texttospeech/client

docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/texttospeech.yaml \
  -g cpp-pistache-server \
  -o /local/gen/c/texttospeech/server

# Python
docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/texttospeech.yaml \
  -g python \
  -o /local/gen/python/texttospeech/client

docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/texttospeech.yaml \
  -g python-flask \
  -o /local/gen/python/texttospeech/server

# --------------
# MQTTTRANSPORT
# --------------
# C
docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/mqtttransport.yaml \
  -g c \
  -o /local/gen/c/mqtttransport/client

docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/mqtttransport.yaml \
  -g cpp-pistache-server \
  -o /local/gen/c/mqtttransport/server

# Python
docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/mqtttransport.yaml \
  -g python \
  -o /local/gen/python/mqtttransport/client

docker run --rm \
  -v ${PWD}:/local openapitools/openapi-generator-cli generate \
  -i /local/interfaces/mqtttransport.yaml \
  -g python-flask \
  -o /local/gen/python/mqtttransport/server
