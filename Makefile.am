bin_SCRIPTS = bufr2json

bufr2json: bufr2json.py
	sed -e 's,[@]PACKAGE_VERSION[@],@PACKAGE_VERSION@,g' $< > $@

check-local:
	python3 test_bufr2json.py

EXTRA_DIST = bufr2json.py
