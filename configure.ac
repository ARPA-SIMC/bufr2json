#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.68])
AC_INIT([bufr2json], [0.16], [edigiacomo@arpae.it])
AC_CONFIG_SRCDIR([configure.ac])

# Checks for programs.
AM_PATH_PYTHON([3.4])
AM_INIT_AUTOMAKE([foreign nostdinc subdir-objects])

AC_PATH_PROG([HELP2MAN], [help2man])
AM_CONDITIONAL([HAVE_HELP2MAN], [test x$HELP2MAN != x])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
