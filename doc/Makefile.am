EXTRA_DIST = bufr2json.1.include

CLEANFILES =

if HAVE_HELP2MAN

man1_MANS = bufr2json.1

bufr2json.1: $(top_builddir)/src/bufr2json $(srcdir)/bufr2json.1.include
	        $(HELP2MAN) --output=$@ --no-info --include=$(srcdir)/bufr2json.1.include --name="BUFR to JSON" $<

CLEANFILES += bufr2json.1

endif
