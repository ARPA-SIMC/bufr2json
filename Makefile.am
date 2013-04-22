bin_PROGRAMS = bufr2json

bufr2json_SOURCES = bufr2json.cc

bufr2json_LDADD = -ldballe -lyajl

CLEANFILES =

if HAVE_HELP2MAN

man1_MANS = bufr2json.1

bufr2json.1: bufr2json.cc bufr2json.1.include 
	$(HELP2MAN) --output=$@ --no-info --include=bufr2json.1.include --name="BUFR to JSON" $(builddir)/bufr2json

CLEANFILES += bufr2json.1

endif
