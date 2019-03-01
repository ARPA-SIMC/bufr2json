#!/usr/bin/python3
import argparse
import sys
import simplejson as json

import dballe


__version__ = '@PACKAGE_VERSION@'


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Convert BUFR files to GeoJSON format")
    parser.add_argument("inputfile", nargs="*", metavar="FILE", help="BUFR file")
    parser.add_argument('-V', '--version', action='version',
                        version='%(prog)s ' + __version__)

    args = parser.parse_args()

    out = sys.stdout

    importer = dballe.Importer("BUFR")

    if not args.inputfile:
        inputfiles = [sys.stdin]
    else:
        inputfiles = args.inputfile

    out.write('{"type":"FeatureCollection", "features":[')
    for f in inputfiles:
        with importer.from_file(f) as fp:
            explorer = dballe.Explorer()
            is_first = True
            for msgs in fp:
                for msg in msgs:
                    with explorer.rebuild() as update:
                        update.add_messages(msg)

                    for l, t, v in (
                        (l, t, v)
                        for l in explorer.all_levels
                        for t in explorer.all_tranges
                        for v in explorer.all_varcodes
                    ):
                        var = msg.get(l, t, v)
                        if var is None:
                            continue
                        if l is None and t is None:
                            continue

                        if not is_first:
                            out.write(",")
                        else:
                            is_first = False

                        json.dump({
                            "type": "Feature",
                            "geometry": {
                                "type": "Point",
                                "coordinates": [msg.coords[1], msg.coords[0]]
                            },
                            "properties": {
                                "lon": msg.get(dballe.Level(), dballe.Trange(), "B05001").enqi(),
                                "lat": msg.get(dballe.Level(), dballe.Trange(), "B06001").enqi(),
                                "datetime": msg.datetime.strftime("%Y-%m-%dT%H:%M:%SZ"),
                                "network": msg.report,
                                "ident": msg.ident,
                                "level_t1": l.ltype1 if l is not None else None,
                                "level_v1": l.l1 if l is not None else None,
                                "level_t2": l.ltype2 if l is not None else None,
                                "level_v2": l.l2 if l is not None else None,
                                "trange_pind": t.pind if t is not None else None,
                                "trange_p1": t.p1 if t is not None else None,
                                "trange_p2": t.p2 if t is not None else None,
                                "bcode": var.code,
                                "value": var.get(),
                            }
                        }, out)

    out.write("]}")
