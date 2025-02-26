# syntax=docker/dockerfile:1
FROM python:3.12-slim-bullseye AS development

ENV PYTHONDONTWRITEBYTECODE=1
ENV PYTHONUNBUFFERED=1

EXPOSE 5000

RUN sh -c "apt-get update \
 && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3-pip \
 && pip install --upgrade pip "

WORKDIR /srv/mqtt-transport/

#RUN pip install -r requirements.txt
RUN pip install flask paho-mqtt apiflask requests

RUN mkdir /srv/mqtt-transport/logs

COPY . .

# Optional: Create directories for persistence and logging
VOLUME ["/srv/mqtt-transport/logs"]

ENTRYPOINT ["flask", "--app", "server.py", "run", "--host=0.0.0.0", "--port=5000"]
