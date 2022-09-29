#!/bin/bash

# This script should always exit with code 42 if the display is on.

# It is used to determine whether the 'q' button should turn the screen on or off when it is
# pressed.

# This script was designed for the RPi VideoCore, but it can be using anything you wish, as
# long as you adhere to the above. Please also check the "hibernate.sh" and "wakeup.sh" scripts
# which are used after this script to either turn the monitor off or on.

if xrandr --listactivemonitors | grep -q 'Monitors: 0'
then
    exit 0
else
    exit 42
fi
