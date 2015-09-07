AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir)

noinst_LTLIBRARIES = libbufr2json.la

noinst_HEADERS = \
				 json.h \
				 dump.h

libbufr2json_la_LIBADD = -lyajl

libbufr2json_la_SOURCES = \
						  json.cc \
						  dump.cc
