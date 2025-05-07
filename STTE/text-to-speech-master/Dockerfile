FROM python:3.11-slim-bullseye AS intermediate

# Set environment variables to avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Set Python 3.11 and Pip 3.11 as default
RUN update-alternatives --install /usr/bin/python3 python3 /usr/local/bin/python3.11 1 && \
    update-alternatives --install /usr/bin/pip3 pip3 /usr/local/bin/pip3.11 1

# Install package requirements
RUN apt-get update && \
    apt-get install -y --no-install-recommends wget xz-utils && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Install ffmpeg manually
RUN wget https://johnvansickle.com/ffmpeg/releases/ffmpeg-release-amd64-static.tar.xz && \
    tar -xJf ffmpeg-release-amd64-static.tar.xz && \
    mv ffmpeg-*-amd64-static/ffmpeg /usr/local/bin/ && \
    mv ffmpeg-*-amd64-static/ffprobe /usr/local/bin/ && \
    rm -rf ffmpeg-*-amd64-static ffmpeg-release-amd64-static.tar.xz

WORKDIR /app

COPY requirements.txt .

RUN pip3 install --upgrade pip && \
    pip3 install --prefix=/install -r requirements.txt && \
    rm -rf /root/.cache/pip /tmp/*


FROM python:3.11-slim-bullseye

# Set Python 3.11
RUN update-alternatives --install /usr/bin/python3 python3 /usr/local/bin/python3.11 1

# Install package requirements
RUN apt-get update && \
    apt-get install -y --no-install-recommends espeak-ng && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

RUN mkdir -p /usr/src/app
RUN mkdir -p /usr/src/app/logs

COPY --from=intermediate /install /usr/local
COPY --from=intermediate /usr/local/bin/ffmpeg /usr/local/bin
COPY --from=intermediate /usr/local/bin/ffprobe /usr/local/bin

WORKDIR /usr/src/app

# Copy the models first (since these do not change often)
COPY models /usr/src/app/models

# Copy the code files
COPY connectors /usr/src/app/connectors
COPY openapi_client /usr/src/app/openapi_client
COPY openapi_server /usr/src/app/openapi_server
COPY text_to_speech /usr/src/app/text_to_speech
COPY test /usr/src/app/test

EXPOSE 8081

ENTRYPOINT ["python3"]

CMD ["-m", "openapi_server"]
#CMD ["test/test.py"]