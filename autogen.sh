#!/bin/bash

if [[ $1 == -re ]]; then
	shift
	mkdir -p m4
	exec autoreconf -fi $@
	exit 0;
elif [[ $1 == -rm ]]; then
	shift
	rm -f aclocal.m4
	rm -f config.guess
	rm -f config.sub
	rm -f depcomp
	rm -f ltmain.sh
	rm -f configure~
	rm -f configure.ac~
	rm -f profiterole/config.h.in
	rm -f missing
	rm -f compile
	rm -f Makefile
	rm -f install-sh
	rm -f config.log
	rm -f config.status
	rm -f libtool
	rm -rf autom4te.cache
	rm -rf build
	rm -rf pyprofiterole.egg-info
	rm -rf m4
	rm -rf $@
	exit 0;
elif [[ $1 == -rx ]]; then
	shift
	libtoolize
	aclocal
	autoheader
	autoconf
	automake --add-missing
	$(@)
	exit 0;
else
	echo "Usage: $(basename $0) [-re ...] or [-rm ...] or [-rx ...]"
	echo "-re  | Invoke autoreconf -fi, extra arguments are passed to it"
	echo "-rm  | Remove all autotools generated files, extra arguments will also be removed files"
	echo "-rx  | Invoke autotools utils in order, extra args will be executed"
	exit 1;
fi

