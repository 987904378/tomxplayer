# Tactical OMXPlayer aka: tomxplayer
C/GTK2 port of tomxplayer /  GUI Video player / Wrapper for omxplayer | For the Raspberry Pi.

# Building
tomxplayer is built and developed on the Raspberry Pi 3 Model B using Ubuntu Mate (16.04 LTS currently).
There is currently, no method of cross compilation and building on architectures other than armhf is not supported.

## Development Dependancies
libgtk2.0-dev (>=2.0), libdbus-1-dev (>=1.10)

## Runtime Dependancies
libgtk2.0-0 (>= 2.0), dbus (>=1.10), omxplayer(>=0.3.7~git20160923~dfea8c9)

## Instructions
Debug build that runs from current directory.
1. make debug

Debian package
1. make release


## NOTE: Because this is developed on the Raspberry Pi and the Rasbperry Pi does not come with a real-time clock, my commits may be oddly out of chronological order.
