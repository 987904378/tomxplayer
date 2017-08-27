# Tactical OMXPlayer aka: tomxplayer
C/GTK2 port of tomxplayer /  GUI Video player / Wrapper for omxplayer | For the Raspberry Pi.

# Building
tomxplayer is built and developed on the Raspberry Pi 3 Model B using Ubuntu Mate (16.04 LTS currently).
There is currently, no method of cross compilation and building on architectures other than armhf is not supported.

## Development Dependancies
libgtk2.0-dev (>=2.24.30)
libdbus-1-dev (>=1.10)
(Afaik this is all)

## Runtime Dependancies
libgtk-2.0 (>=2.24.20)
dbus (>=1.10)
(Afaik this is all)

## Instructions
Debug build that runs from current directory.
1. make debug

Debian package
1. make release
