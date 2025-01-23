FROM python:3.10

WORKDIR /usr/src/app

ENV UNAME pulse

# Install PulseAudio
RUN apt-get update && \
    apt-get install -y pulseaudio liblinphone-dev sqlite3 dbus libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0 && \
    rm -rf /var/lib/apt/lists/*

COPY requirements.txt requirements.txt

RUN pip3 install -r requirements.txt

RUN mkdir -p /home/${UNAME}/.local/share/linphone

COPY client.conf /etc/pulse/client.conf

RUN export UNAME=$UNAME UID=1000 GID=1000 && \
    mkdir -p "/home/${UNAME}" && \
    echo "${UNAME}:x:${UID}:${GID}:${UNAME} User,,,:/home/${UNAME}:/bin/bash" >> /etc/passwd && \
    echo "${UNAME}:x:${UID}:" >> /etc/group && \
    mkdir -p /etc/sudoers.d && \
    echo "${UNAME} ALL=(ALL) NOPASSWD: ALL" > /etc/sudoers.d/${UNAME} && \
    chmod 0440 /etc/sudoers.d/${UNAME} && \
    chown ${UID}:${GID} -R /home/${UNAME} && \
    gpasswd -a ${UNAME} audio

EXPOSE 5060 5060
EXPOSE 7078 7078
EXPOSE 9078 9078
EXPOSE 11078 11078

# Copy your application and any necessary scripts
COPY linphone_module.cpython-310-x86_64-linux-gnu.so linphone_module.cpython-310-x86_64-linux-gnu.so
COPY test.py test.py

COPY entrypoint.sh /entrypoint.sh
RUN chmod +x /entrypoint.sh

RUN chmod -R 777 /home/${UNAME}
RUN chown -R pulse:pulse /home/${UNAME}
RUN chown -R pulse:pulse /usr/src/app

USER ${UNAME}
ENV HOME /home/${UNAME}

# Use an entrypoint script to start PulseAudio daemon and then your application
ENTRYPOINT ["/entrypoint.sh"]

CMD ["python3", "/usr/src/app/test.py"]