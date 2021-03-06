#!/usr/bin/python3
import argparse
import sys
import json
import dballe


__version__ = '@PACKAGE_VERSION@'


def main(inputfiles, out):

    importer = dballe.Importer("BUFR")

    out.write('{"type":"FeatureCollection", "features":[')
    for f in inputfiles:
        with importer.from_file(f) as fp:
            is_first = True
            for msgs in fp:
                for msg in msgs:
                    for cur in msg.query_data():
                        lev = cur["level"]
                        tr = cur["trange"]

                        if not is_first:
                            out.write(",")
                        else:
                            is_first = False

                        var = cur["variable"]
                        json.dump({
                            "type": "Feature",
                            "geometry": {
                                "type": "Point",
                                "coordinates": [cur.enqd("lon"), cur.enqd("lat")],
                            },
                            "properties": {
                                "lon": cur.enqi("lon"),
                                "lat": cur.enqi("lat"),
                                "datetime": cur["datetime"].strftime("%Y-%m-%dT%H:%M:%SZ"),
                                "network": cur["report"],
                                "ident": cur["ident"],
                                "level_t1": lev.ltype1 if lev is not None else None,
                                "level_v1": lev.l1 if lev is not None else None,
                                "level_t2": lev.ltype2 if lev is not None else None,
                                "level_v2": lev.l2 if lev is not None else None,
                                "trange_pind": tr.pind if tr is not None else None,
                                "trange_p1": tr.p1 if tr is not None else None,
                                "trange_p2": tr.p2 if tr is not None else None,
                                "bcode": var.code,
                                "value": var.get(),
                            }
                        }, out)

    out.write("]}")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Convert BUFR files to GeoJSON format")
    parser.add_argument("inputfile", nargs="*", metavar="FILE", help="BUFR file")
    parser.add_argument('-V', '--version', action='version',
                        version='%(prog)s ' + __version__)

    args = parser.parse_args()

    if not args.inputfile:
        inputfiles = [sys.stdin]
    else:
        inputfiles = args.inputfile

    main(inputfiles, sys.stdout)
