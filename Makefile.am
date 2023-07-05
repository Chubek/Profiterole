AUTOMAKE_OPTIONS = subdir-objects
LDADD = $(LIBOBJS) $(ALLOCA)
AM_CCFLAGS = $(all_includes)
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libprofiterole.la

libprofiterole_la_LDFLAGS = -lrt
libprofiterole_la_SOURCES = $(srcdir)/include/custardmacs.h $(srcdir)/include/profiterole.h $(srcdir)/src/profiterole.c

