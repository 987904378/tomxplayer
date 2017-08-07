# Tactical OMXPlayer aka: tomxplayer
C/GTK+ port of tomxplayer / GUI Video player / Wrapper for omxplayer | For the Raspberry Pi.

# Building
tomxplayer is built and developed on the Raspberry Pi 3 Model B using Ubuntu Mate (16.04 LTS currently).
There is currently, no method of cross compilation and building on architectures other than armhf is not supported.
The version for Raspbian (Stretch currently) is GTK2 and therefore requires the GTK2 development and runtime dependancies. Likewise,
Ubuntu Mate is GTK3 and requires the GTK3 development and runtime dependancies.

## Development Dependancies
* GTK3: libgtk-3-dev (>=3.18)
* GTK2: libgtk2.0-dev (>=2.0)
* libdbus-1-dev (>=1.10)`
* Raspberry Pi specific development files located:
	* https://github.com/raspberrypi/firmware
	* Specifically the files in /opt/vc
	* In Raspbian Stretch and Ubuntu Mate 16.04 LTS, these files are already included.

## Runtime Dependancies
* GTK3: libgtk-3-0 (>=3.18)
* GTK2: libgtk2.0-0 (>= 2.0)
* ALL: dbus (>=1.10), omxplayer(>=0.3.7~git20160923~dfea8c9)
* Raspberry Pi specific development files located:
	* https://github.com/raspberrypi/firmware
	* Specifically the files in /opt/vc
	* In Raspbian Stretch and Ubuntu Mate 16.04 LTS, these files are already included.	

## Ubuntu Mate Instructions
Debug build that runs from current directory.
1. make mate-debug

Debian package
1. make mate-release

## Raspbian Instructions
Debug build that runs from current directory.
1. make raspi-debug

Debian package
1. make raspi-release


## NOTE: Because this is developed on the Raspberry Pi and the Rasbperry Pi does not come with a real-time clock, my commits may be oddly out of chronological order.
