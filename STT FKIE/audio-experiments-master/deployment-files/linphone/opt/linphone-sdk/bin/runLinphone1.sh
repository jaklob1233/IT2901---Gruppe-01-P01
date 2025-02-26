#!/bin/sh
export PATH="/opt/linphone-sdk/bin:$PATH"

linphonecsh soundcard use files

# call
echo 'call sip:10.10.0.2'
if linphonecsh generic "call sip:10.10.0.2" | grep -q 'in progress'; then
  echo 'call in progress'
fi

# todo handle delay of STT initialization in filter init method
sleep 5

# play
echo 'play audio file'
FILENAME=/opt/linphone-sdk/audio-samples/$1
linphonecsh generic "play $FILENAME"
DURATION=$(ffprobe -i "$FILENAME" -show_entries format=duration -v quiet -of csv="p=0")

echo "filename=$FILENAME"

echo "sleep $DURATION"
sleep "$DURATION"

# terminate
echo 'terminate call'
linphonecsh generic terminate # [callId]
