# Tactical OMXPlayer aka: tomxplayer
C/GTK+ port of tomxplayer /  GUI Video player / Wrapper for omxplayer | For the Raspberry Pi.

# Building
tomxplayer is built and developed on the Raspberry Pi 3 Model B using Ubuntu Mate (16.04 LTS currently).
There is currently, no method of cross compilation and building on architectures other than armhf is not supported.
tomxplayer can be built for either GTK2 or GTK3 by Changing th GTK_VERSION flag at the top of 'Makefile'. The
flag is '3' (for GTK3) by default.

## Development Dependancies
* GTK3: libgtk3.0-dev (>=3.18)
* GTK2: libgtk2.0-dev (>=2.0)
* libdbus-1-dev (>=1.10)`
* Raspberry Pi specific development files located:
	* https://github.com/raspberrypi/firmware
	* Specifically the files in /opt/vc

## Runtime Dependancies
* GTK3: libgtk3.0 (>=3.18)
* GTK2: libgtk2.0-0 (>= 2.0)
* ALL: dbus (>=1.10), omxplayer(>=0.3.7~git20160923~dfea8c9)
* Raspberry Pi specific development files located:
	* https://github.com/raspberrypi/firmware
	* Specifically the files in /opt/vc

## Instructions
Debug build that runs from current directory.
1. make debug

Debian package
1. make release


## NOTE: Because this is developed on the Raspberry Pi and the Rasbperry Pi does not come with a real-time clock, my commits may be oddly out of chronological order.
