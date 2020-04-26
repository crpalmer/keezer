#!/bin/bash

# Add this to /etc/rc.local:
#
# /opt/keezer/run-temp-controller.sh

(
while true
do
    cd /opt/keezer
    ./temp-controller temp-controller-configs/`cat temp-controller.cfg`.cfg
    sleep 2
done
) > /tmp/temp-controller.log 2>&1 &
