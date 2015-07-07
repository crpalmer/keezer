#!/bin/bash

lxpanel --profile LXDE
pcmanfm --desktop --profile LXDE
xscreensaver -no-splash

xset s off
xset -dpms
xset s noblank
mkdir /tmp/keeezer-temporary-dir
chromium --user-data-dir=/tmp/keezer-temporary-dir --kiosk http://pi:3000
