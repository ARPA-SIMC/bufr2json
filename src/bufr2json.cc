/*
 * Copyright (C) 2013-2015  ARPA-SIM <urpsim@smr.arpa.emr.it>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Author: Emanuele Di Giacomo <edigiacomo@arpa.emr.it>
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdlib>
#include <iostream>
#include <sstream>

#include <dballe/core/var.h>
#include <dballe/msg/msg.h>
#include <dballe/msg/context.h>
#include <dballe/cmdline/processor.h>

#include <yajl/yajl_version.h>
#include <yajl/yajl_gen.h>

#include <bufr2json/json.h>
#include <bufr2json/dump.h>


#include <getopt.h>

void show_help(std::ostream& out) {
    out << "Usage: bufr2json [OPTION]... [FILE]..."  << std::endl
        << "Convert BUFR files to JSON format" << std::endl
        << std::endl
        << " -f,--format=FORMAT     JSON output format (geojson, dballe. Default: geojson)" << std::endl
        << " -h,--help           show this help and exit" << std::endl
        << " -V,--version        show version and exit" << std::endl
        << std::endl
        << "With no FILE read standard input" << std::endl;
}

void show_version(std::ostream& out) {
    out << "bufr2json " << PACKAGE_VERSION << std::endl;
}

int main(int argc, char **argv)
{
    struct {
        std::string format;
    } input_options = {
        "geojson",
    };
    std::list<std::string> inputlist;

    while (1) {
        static struct option long_options[] = {
            {"format",          required_argument,0,                     'f'  },
            {"help",            no_argument,0,                           'h'  },
            {"version",         no_argument,0,                           'V'  },
            {0, 0, 0, 0 }
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "hVf:", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 0: break;
            case 'h': show_help(std::cout); return 0;
            case 'V': show_version(std::cout); return 0;
            case 'f': {
                input_options.format = optarg;
                break;
            }
            default: show_help(std::cerr); return 1;
        }
    }
    while (optind < argc) {
        std::string f(argv[optind++]);
        if (f == "-") f = "";
        inputlist.push_back(f);
    }

    dballe::cmdline::Reader reader;

    using bufr2json::dump::JsonDumper;
    using bufr2json::dump::GeoJsonDumper;
    using bufr2json::dump::DballeJsonDumper;

    std::unique_ptr<JsonDumper> dumper;
    if (input_options.format == "geojson")
        dumper.reset(new GeoJsonDumper(std::cout));
    else if (input_options.format == "dballe")
        dumper.reset(new DballeJsonDumper(std::cout));
    else {
        std::cerr << "Invalid JSON format: " << input_options.format << std::endl;
        return 1;
    }

    dumper->start_dump();
    reader.read(inputlist, *dumper);
    dumper->end_dump();
    dumper->flush();

    return 0;
}
