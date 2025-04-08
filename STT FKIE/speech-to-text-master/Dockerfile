FROM python:3.11-slim-bullseye AS intermediate

# Set environment variables to avoid interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Set Python 3.11 and Pip 3.11 as default
RUN update-alternatives --install /usr/bin/python3 python3 /usr/local/bin/python3.11 1 && \
    update-alternatives --install /usr/bin/pip3 pip3 /usr/local/bin/pip3.11 1

# Install package requirements
# portaudio (needed for pyaudio)
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    ca-certificates \
    software-properties-common \
    wget \
    xz-utils \
    portaudio19-dev \
    python3-dev \
    build-essential \
    git \
    curl \
    unzip && \
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
    pip3 install --prefix=/install -r requirements.txt

# locate the file and change the paths
ENV SITE_PACKAGE_PATH="/install/lib/python3.11/site-packages"
ENV OPENAI_PUBLIC_FILE="${SITE_PACKAGE_PATH}/tiktoken_ext/openai_public.py"

ENV BASE_URL_TO_REPLACE="https://openaipublic.blob.core.windows.net"
ENV GPT2_URL_TO_REPLACE="${BASE_URL_TO_REPLACE}/gpt-2/encodings/main"
ENV ENCODINGS_URL_TO_REPLACE="${BASE_URL_TO_REPLACE}/encodings"

ENV BASE_LOCAL_PATH="/usr/src/app/speech_to_text"
ENV ENCODINGS_LOCAL_PATH="${BASE_LOCAL_PATH}/encodings"

RUN sed -i "s|${GPT2_URL_TO_REPLACE}|${ENCODINGS_LOCAL_PATH}|g" ${OPENAI_PUBLIC_FILE} && \
    sed -i "s|${ENCODINGS_URL_TO_REPLACE}|${ENCODINGS_LOCAL_PATH}|g" ${OPENAI_PUBLIC_FILE}

# download speech-to-text models
# Download Vosk multilingual model (0.3)
#RUN curl https://alphacephei.com/vosk/models/vosk-model-multilingual-0.3.zip -o vosk_model_multilingual_medium.zip && \
#    unzip vosk_model_multilingual.zip -d /models/vosk_multilingual_medium && \
#    rm vosk_model_multilingual_medium.zip

# Download Whisper multilingual base model
RUN mkdir -p /models/whisper_multilingual_base && \
    curl https://openaipublic.azureedge.net/main/whisper/models/ed3a0b6b1c0edf879ad9b11b1af5a0e6ab5db9205f891f668f8b0e6c6326e34e/base.pt -o /models/whisper_multilingual_base/base.pt

# Download vosk medium english
RUN mkdir -p  /models/vosk_english_medium && \
    curl https://alphacephei.com/vosk/models/vosk-model-en-us-0.22-lgraph.zip -o vosk_medium.zip && \
    vosk_model_name=$(unzip -qql vosk_medium.zip | head -n1 | tr -s ' ' | cut -d' ' -f5-) && \
    unzip vosk_medium.zip && \
    cp -a $vosk_model_name/. /models/vosk_english_medium && \
    rm -rf $vosk_model_name && \
    rm vosk_medium.zip

# Download whisper base english
RUN mkdir -p /models/whisper_english_base && \
    curl https://openaipublic.azureedge.net/main/whisper/models/25a8566e1d0c1e2231d1c762132cd20e0f96a85d16145c3a00adf5d1ac670ead/base.en.pt -o /models/whisper_english_base/base.en.pt && \
    curl https://openaipublic.azureedge.net/main/whisper/models/ed3a0b6b1c0edf879ad9b11b1af5a0e6ab5db9205f891f668f8b0e6c6326e34e/base.pt -o /models/whisper_english_base/base.pt

# download speech-to-text encodings
RUN mkdir -p /encodings && \
    curl https://openaipublic.blob.core.windows.net/gpt-2/encodings/main/vocab.bpe -o /encodings/vocab.pbe && \
    curl https://openaipublic.blob.core.windows.net/gpt-2/encodings/main/encoder.json -o /encodings/encoder.json && \
    curl https://openaipublic.blob.core.windows.net/encodings/r50k_base.tiktoken -o /encodings/r50k_base.tiktoken && \
    curl https://openaipublic.blob.core.windows.net/encodings/p50k_base.tiktoken -o /encodings/p50k_base.tiktoken && \
    curl https://openaipublic.blob.core.windows.net/encodings/cl100k_base.tiktoken -o /encodings/cl100k_base.tiktoken && \
    curl https://openaipublic.blob.core.windows.net/encodings/o200k_base.tiktoken -o /encodings/o200k_base.tiktoken



FROM python:3.11-slim-bullseye

# Set Python 3.11
RUN update-alternatives --install /usr/bin/python3 python3 /usr/local/bin/python3.11 1

RUN apt-get update && \
    apt-get install -y --no-install-recommends portaudio19-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

COPY --from=intermediate /install /usr/local
COPY --from=intermediate /usr/local/bin/ffmpeg /usr/local/bin
COPY --from=intermediate /usr/local/bin/ffprobe /usr/local/bin

#copy the code files
RUN mkdir -p /usr/src/app
RUN mkdir -p /usr/src/app/logs

COPY --from=intermediate /models /usr/src/app/models
COPY --from=intermediate /encodings /usr/src/app/encodings

WORKDIR /usr/src/app

# Copy the code files
COPY connectors /usr/src/app/connectors
COPY openapi_client /usr/src/app/openapi_client
COPY openapi_server /usr/src/app/openapi_server
COPY speech_to_text /usr/src/app/speech_to_text
COPY test /usr/src/app/test

EXPOSE 8081

ENTRYPOINT ["python3"]

CMD ["-m", "openapi_server"]