SUBDIRS = embedded/libgeohash

bin_PROGRAMS = bufr2json

bufr2json_SOURCES = bufr2json.cc

bufr2json_CPPFLAGS = -I$(top_srcdir) -I$(top_builddir) -I$(top_srcdir)/embedded/libgeohash

bufr2json_LDADD = -ldballe -lyajl $(top_builddir)/embedded/libgeohash/libgeohash.la

CLEANFILES =

EXTRA_DIST = bufr2json.1.include

if HAVE_HELP2MAN

man1_MANS = bufr2json.1

bufr2json.1: $(builddir)/bufr2json $(srcdir)/bufr2json.1.include 
	$(HELP2MAN) --output=$@ --no-info --include=$(srcdir)/bufr2json.1.include --name="BUFR to JSON" $(builddir)/bufr2json

CLEANFILES += bufr2json.1

endif
