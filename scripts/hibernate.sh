#!/bin/bash
# The for loop is necessary due to some monitors instantly sending a
# reconnect right after turning the monitor off. This results in the
# monitor turning back on.
for i in {1..15}
do
xrandr --output HDMI-1 --off
sleep 1
done
