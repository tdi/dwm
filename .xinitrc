#!/bin/bash
xrdb -merge ~/.Xresources
export XDG_MENU_PREFIX="lxde-"
export DE=LXDE

if [ -f $HOME/.Xmodmap]; then
  /usr/bin/xmodmap ~/.Xmodmap
fi
ck-launch-session nm-applet  &
# xbindkeys &
/home/tdi/bin/batterymon.sh & 
 setxkbmap pl
 xinput set-prop 'TPPS/2 IBM TrackPoint' "Evdev Wheel Emulation" 1
 xinput set-prop 'TPPS/2 IBM TrackPoint' "Evdev Wheel Emulation Button" 2
 xinput set-prop 'TPPS/2 IBM TrackPoint' "Evdev Wheel Emulation Timeout" 200
 start-pulseaudio-x11
 /usr/lib/x86_64-linux-gnu/lxpolkit &
 # exec qtile
 udiskie &
while true; do 
  /home/tdi/dev/dwmstatus/dwmstatus
  sleep 1m
done &
conky &
xmodmap ~/.Xmodmap
xscreensaver -no-splash &
feh --bg-scale /home/tdi/share/wallpapers/dwm.png 
exec dbus-launch /home/tdi/dev/dwm-6.1/dwm

#exec dbus-launch awesome 2> /home/tdi/awesome.log
