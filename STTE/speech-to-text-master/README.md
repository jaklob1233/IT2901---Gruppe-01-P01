# Speech-to-Text

## Name
speech-to-text

## Description
This project implements a speech-to-text component. It gets audio via a REST-API and provides text to the calling component.
The calling component first calls the initialize method and provides a webhook url. The results are sent to the webhook url which is also a REST server.
The openapi spec is in [OpenAPI Spec](openapi_server/openapi/openapi.yaml).

## Installation
for installing locally:

    docker build . -t speechtotext:latest

## Further development

## Usage
    docker run --rm --network host --name speechtotext -v /home/toor/projects/IST-201/speech-to-text/logs:/usr/src/app/logs speechtotext:latest
adapt /home/toor/projects/IST-201/speech-to-text/logs to correct folder

## Authors and acknowledgment
Wiam Rachid, Fraunhofer FKIE, 2024

Norman JANSEN, Fraunhofer FKIE, 2024
