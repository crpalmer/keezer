#!/bin/bash

# Add this to /etc/rc.local:
#
# sudo -u crpalmer /opt/keezer/run-ui.sh

(
source /home/crpalmer/.rvm/scripts/rvm
rvm use ruby

cd /home/crpalmer/keezer/ui
while true
do
    ./bin/rails server -b 0.0.0.0
    sleep 2
done
) > /tmp/keezer-ui.log 2>&1
