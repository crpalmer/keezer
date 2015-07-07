#!/bin/sh

cd /home/crpalmer/keezer/ui
while true
do
    sudo -u crpalmer ./bin/rails server -b 0.0.0.0
    sleep 2
done
