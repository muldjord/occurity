#!/bin/bash
# 'xrandr' seems unstable on some monitors. Let's try with 'xset' instead
#xrandr --output HDMI-1 --auto
xset dpms force on
