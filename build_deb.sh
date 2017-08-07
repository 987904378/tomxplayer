#!/bin/bash

CONF=$1
HOME_PATH=$PWD
VERSION=0

VERSION=$(./tomxplayer.bin -v)
echo tomxplayer version $VERSION
cd $HOME_PATH
INT_DIR="/tmp/tomxplayer_$VERSION"
rm -rf $INT_DIR
mkdir -p -v $INT_DIR
mkdir -p -v $INT_DIR/DEBIAN
cat DEBIAN/control | sed "s/%version%/$VERSION/g"  > $INT_DIR/DEBIAN/control
mkdir -p $INT_DIR/usr/share/applications
sed "s/####/$VERSION/g" < DEBIAN/tomxplayer-gtk.desktop > $INT_DIR/usr/share/applications/tomxplayer-gtk.desktop
mkdir -p -v $INT_DIR/usr/bin
mkdir -p -v $INT_DIR/usr/share/tomxplayer/osd_rpi
cp -v osd_rpi/Vera.ttf $INT_DIR/usr/share/tomxplayer/osd_rpi
cp -v tomxplayer.png $INT_DIR/usr/share/tomxplayer
cp -v tomxplayer $INT_DIR/usr/bin
cp -v tomxplayer.bin $INT_DIR/usr/bin

remove_old_builds() {
	cd $HOME_PATH/out
	rm *.deb
}

create_deb() {
	remove_old_builds
	dpkg-deb -v --build $INT_DIR

	cd $HOME_PATH

	mkdir -p out/
	mv -v /tmp/tomxplayer_$VERSION.deb out/tomxplayer_$VERSION.deb

}

create_deb

rm -rf $INT_DIR



