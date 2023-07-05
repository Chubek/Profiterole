AC_INIT([profiterole],[1.0],[chubakbidpaa@gmail.om])
AC_CONFIG_MACRO_DIRS([m4])
AC_CONFIG_SRCDIR([profiterole/config.h])
AC_CONFIG_HEADERS([profiterole/config.h])
AC_CONFIG_FILES([Makefile])
AM_INIT_AUTOMAKE([foreign])
LT_INIT([shared])
AC_LANG([C])
AC_PROG_CC
AC_PROG_CXX
AC_CHECK_LIB([rt], AC_MSG_ERROR([librt must be provided]))
AC_CHECK_INCLUDES_DEFAULT
AC_CHECK_TYPES([ssize_t size_t pid_t mqd_t])
AC_CHECK_SIZEOF([unsigned long])
AC_ARG_ENABLE([memcpy],
		[AS_HELP_STRING(
			[--enable-memcpy],
			[Use memcpy instead of memmove when needed])],
			[AC_DEFINE([WANT_MEMCPY], [1], [Set to 1 if you wish to use memcpy instead of memmove])
			])
AC_ARG_ENABLE([asmfuncs], 
		[AS_HELP_STRING(
			[--enable-asmfuncs],
			[Use the inline-Assembly functions when needed])], 
			[AC_DEFINE([WANT_ASM_FUNCS], [1], [Set to 1 if you wish inline Assembly functions to be used])
			])
AC_OUTPUT
