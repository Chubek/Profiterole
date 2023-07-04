#!/bin/bash

if [[ $1 == -re ]]; then
	autoreconf
	exit;
elif [[ $1 == -rm ]]; then
	rm -f aclocal.m4
	rm -f config.guess
	rm -f config.sub
	rm -f configure
	rm -f depcomp
	rm -f ltmain.sh
	rm -f configure~
	rm -f configure.ac~
	rm -rf autom4te.cache
	rm -rf build
	rm -rf pyprofiterole.egg-info
	rm -rf m4
	exit;
fi

libtoolize
aclocal
automake
autoconf
