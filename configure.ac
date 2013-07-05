#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([bufr2json], [0.4], [edigiacomo@arpa.emr.it])
AC_CONFIG_SRCDIR([configure.ac])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CXX
AM_INIT_AUTOMAKE([foreign nostdinc subdir-objects])
LT_INIT

PKG_CHECK_MODULES([DBALLE], [libdballe >= 6.0])
LIBS="$LIBS $DBALLE_LIBS"
CFLAGS="$CFLAGS $DBALLE_CFLAGS"

AC_CHECK_LIB([yajl], [yajl_gen_alloc])

AC_PATH_PROG([HELP2MAN], [help2man])
AM_CONDITIONAL([HAVE_HELP2MAN], [test x$HELP2MAN != x])

AC_CONFIG_FILES([Makefile
		 embedded/libgeohash/Makefile
		 src/Makefile
		 doc/Makefile
		 test/Makefile])
AC_OUTPUT
