#!/bin/sh
pid=$(ps -e | pgrep tcpdump)
echo $pid
echo toor | sudo -S kill -2 $pid

echo 'Tcpdump stopped'