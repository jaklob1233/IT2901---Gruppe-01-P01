#!/bin/sh
# start tcpdump to capture the traffic
echo 'start Tcpdump'
echo toor | sudo -S tcpdump -i ens37 -nn -s0 -v -w $1
