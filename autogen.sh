#!/bin/bash

if [ -e ./remakesourcelist ];then
	./remakesourcelist
fi

aclocal
autoheader
touch NEWS README AUTHORS ChangeLog
automake --add-missing --copy

case $USEQTVERSION in
	"5")
		autoconf configure.qt5.ac > ./configure
		chmod +x ./configure
		;;
	*)
		autoconf
		;;
esac
./configure $@

