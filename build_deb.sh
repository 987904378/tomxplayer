#!/bin/bash

CONF=$1
HOME_PATH=$PWD
VERSION=0
SIZE=0
DEPS="";
add_sizeof_file() {
	THISSIZE=$(ls -la --block-size=1024 $1 | cut -d ' ' -f5)
	SIZE=$(expr $SIZE + $THISSIZE)
}

if [[ "$CONF" == "raspi" ]]; then
	DEPS="libgtk2.0-0 (>= 2.0), libdbus-1-3 (>=1.10), omxplayer(>=0.3.7~git20160923~dfea8c9)"
else
	DEPS="libgtk-3-0 (>=3.18), libdbus-1-3 (>=1.10), omxplayer(>=0.3.7~git20160923~dfea8c9)"
fi

VERSION=$(./tomxplayer.bin -v)
echo tomxplayer version $VERSION
cd $HOME_PATH
INT_DIR="/tmp/tomxplayer_$VERSION"
rm -rf $INT_DIR
mkdir -p -v $INT_DIR
mkdir -p -v $INT_DIR/DEBIAN
mkdir -p $INT_DIR/usr/share/applications
sed "s/####/$VERSION/g" < DEBIAN/tomxplayer-gtk.desktop > $INT_DIR/usr/share/applications/tomxplayer-gtk.desktop
mkdir -p -v $INT_DIR/usr/bin
mkdir -p -v $INT_DIR/usr/share/tomxplayer/osd_rpi
cp -v osd_rpi/Vera.ttf $INT_DIR/usr/share/tomxplayer/osd_rpi
add_sizeof_file osd_rpi/Vera.ttf
cp -v tomxplayer_$CONF.png $INT_DIR/usr/share/tomxplayer/tomxplayer.png
add_sizeof_file $INT_DIR/usr/share/tomxplayer/tomxplayer.png
cp -v tomxplayer $INT_DIR/usr/bin
add_sizeof_file tomxplayer
cp -v tomxplayer.bin $INT_DIR/usr/bin
add_sizeof_file tomxplayer.bin
cat DEBIAN/control | sed "s/%version%/$VERSION/g" | sed "s/%size%/$SIZE/g" | sed "s/%deps%/$DEPS/g" > $INT_DIR/DEBIAN/control

remove_old_builds() {
	cd $HOME_PATH/out/$CONF
	rm *.deb
}

create_deb() {
	remove_old_builds
	dpkg-deb -v --build $INT_DIR

	cd $HOME_PATH

	mkdir -p out/$CONF
	mv -v /tmp/tomxplayer_$VERSION.deb out/$CONF/tomxplayer_$VERSION.deb

}

create_deb

rm -rf $INT_DIR



