#!/bin/bash -x

if [[ $1 == -re ]]; then
	shift
	mkdir -p m4
	exec autoreconf -fi $@
	exit;
elif [[ $1 == -rm ]]; then
	shift
	rm -f aclocal.m4
	rm -f config.guess
	rm -f config.sub
	rm -f configure
	rm -f depcomp
	rm -f ltmain.sh
	rm -f configure~
	rm -f configure.ac~
	rm -f profiterole/config.h.in
	rm -f missing
	rm -f compile
	rm -f install-sh
	rm -rf autom4te.cache
	rm -rf build
	rm -rf pyprofiterole.egg-info
	rm -rf m4
	rm -rf $@
	exit;
elif [[ $1 == -rx ]]; then
	shift
	libtoolize
	aclocal
	autoheader
	autoconf
	automake --add-missing
	$(@)
	exit;
fi

