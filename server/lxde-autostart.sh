#!/bin/bash

(
xset s off
xset -dpms
xset s noblank
mkdir /tmp/keeezer-temporary-dir
chromium-browser --user-data-dir=/tmp/keezer-temporary-dir --kiosk http://127.0.0.1:3000
) > /tmp/chromium.log 2>&1
