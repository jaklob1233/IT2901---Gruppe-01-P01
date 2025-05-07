#!/bin/bash
echo "start"
date
linphonecsh init -c /home/toor/.linphonec -d 6 -l /opt/linphone-sdk/linphonec.log
echo "done"
sleep infinity