bin_PROGRAMS = bufr2json

AM_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/embedded/libgeohash

bufr2json_SOURCES = bufr2json.cc

bufr2json_LDADD = -ldballe $(top_builddir)/embedded/libgeohash/libgeohash.la $(top_builddir)/bufr2json/libbufr2json.la
