#!/bin/sh
export PATH="/opt/linphone-sdk/bin:$PATH"

linphonecsh soundcard use files

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
linphonecsh generic "record /opt/linphone-sdk/received-audio/$1"

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
