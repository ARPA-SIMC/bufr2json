#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([bufr2json], [0.1], [edigiacomo@arpa.emr.it])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AM_INIT_AUTOMAKE([foreign nostdinc subdir-objects])

PKG_CHECK_MODULES([DEVEL], [libdballe])
LIBS="$LIBS $DEVEL_LIBS"
CFLAGS="$CFLAGS $DEVEL_CFLAGS"

AC_CHECK_LIB([yajl], [yajl_gen_alloc])

AC_PATH_PROG([HELP2MAN], [help2man])
AM_CONDITIONAL([HAVE_HELP2MAN], [test x$HELP2MAN != x])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
