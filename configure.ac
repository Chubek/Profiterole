AC_INIT([profiterole],[1.0],[chubakbidpaa@gmail.com])
AC_CONFIG_SRCDIR([profiterole/src/profiterole.c])
AM_INIT_AUTOMAKE([foreign])
LT_INIT([shared])
AC_LANG([C])
AC_CONFIG_MACRO_DIRS([m4])
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_CHECK_LIB([rt])
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP
AC_CHECK_HEADERS([unistd.h signal.h mqueue.h stdio.h stdlib.h fcntl.h])
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_TYPE([mqd_t], [], [AC_DEFINE([MUST_DEFINE_MQD_T])], [#include <mqueue.h>)
AC_CHECK_SIZEOF([unsigned long])
AC_ARG_ENABLE([memcpy],
		[AS_HELP_STRING(
		[--enable-memcpy],
		[Use memcpy instead of memmove when needed]
		)}],
		AC_DEFINE([WANT_MEMCPY], [1], [Define if you wish to use memcpy])
		)
AC_ARG_ENABLE([asmfuncs], 
		[AS_HELP_STRING(
			[--enable-asmfuncs]
			[Use the inline-Assembly functions when needed])], AC_DEFINE([WANT_ASM_FUNCS], [1], [Define if you wish inline Assembly functions to be used]))
AC_CONFIG_FILES([Makefile])
AC_OUTPUT