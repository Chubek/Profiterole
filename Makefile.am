AUTOMAKE_OPTIONS = subdir-objects
AM_CCFLAGS = -I$(srcdir)/include
AM_CPPFLAGS = -I$(srcdir)/include
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libprofiterole.la
libprofiterole_la_LDFLAGS = -lrt
libprofiterole_la_SOURCES = $(srcdir)/src/profiterole.c
include_HEADERS = $(srcdir)/include/profiterole.h
