/*
 * Copyright (C) 2013,2014  ARPA-SIM <urpsim@smr.arpa.emr.it>
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
 * Author: Emanuele Di Giacomo
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cstdlib>
#include <iostream>

#include <dballe/msg/msgs.h>
#include <dballe/msg/context.h>
#include <dballe/cmdline/processor.h>

#include <yajl/yajl_version.h>
#include <yajl/yajl_gen.h>

#include <geohash.h>

#define BUFR2JSON_DEFAULT_GEOHASH_SIZE 12

/**
 * Simple C++ wrapper for yajl_gen
 */
class YajlGenerator {
 private:
  yajl_gen json;

 public:
  YajlGenerator(bool beautify, const char* indent=NULL) {
#if YAJL_MAJOR == 1
      yajl_gen_config conf = {beautify, indent};
      json = yajl_gen_alloc(&conf, NULL);
#elif YAJL_MAJOR == 2
      json = yajl_gen_alloc(NULL);
      yajl_gen_config(json, yajl_gen_beautify, beautify);
      if (indent)
          yajl_gen_config(json, yajl_gen_indent_string, indent);
#endif
  }
  ~YajlGenerator() {
      yajl_gen_free(json);
  }
  std::string flush() {
      const unsigned char* buf;
#if YAJL_MAJOR == 1
      unsigned int len;
#elif YAJL_MAJOR == 2
      size_t len;
#endif
      yajl_gen_get_buf(json, &buf, &len);
      std::string str((const char*)buf);
      yajl_gen_clear(json);
      return str;
  }
  void start_list() {
      yajl_gen_array_open(json);
  }
  void end_list() {
      yajl_gen_array_close(json);
  }
  void start_map() {
      yajl_gen_map_open(json);
  }
  void end_map() {
      yajl_gen_map_close(json);
  }
  void add_null() {
      yajl_gen_null(json);
  }
  void add_bool(bool val) {
      yajl_gen_bool(json, val);
  }
  void add_int(long int val) {
      yajl_gen_integer(json, val);
  }
  void add_double(double val) {
      yajl_gen_double(json, val);
  }
  void add_string(const std::string& val) {
      yajl_gen_string(json, (const unsigned char*)val.c_str(), val.size());
  }
  void add_number(const std::string& val) {
      yajl_gen_number(json, val.c_str(), val.size());
  }
};

/**
 * Options for the dumper
 */
struct DumperOptions {
    /// if true, beautify the output
    bool beautify;
    /// if true, ignore empty messages
    bool ignore_empty;
    /// if true, output the station contexts
    bool station_ctx;
    /// if true, collapse the properties
    bool collapse;
    // if true, output the attributes
    bool attributes;
    // if positive, dump geohash
    int geohash;
    // if true, skip invalid data
    bool skip_invalid;

    DumperOptions() : beautify(false), ignore_empty(true), station_ctx(false), collapse(true), attributes(false), geohash(0), skip_invalid(true) {}
};

/**
 * GeoJSON dumper
 *
 * @see http://www.geojson.org/
 */
struct GeoJSONDumper : public dballe::cmdline::Action {
 private:
  std::ostream& out;
  DumperOptions opts;
  YajlGenerator json;
 private:
  void flush() {
      out << json.flush();
  }
  bool is_empty_message(const dballe::Msg& msg) {
      if (msg.data.empty())
          return true;
      if (msg.data.size() == 1 && msg.data.at(0)->is_station())
          return true;
      return false;
  }
  void dump(const dballe::Msgs& msgs) {
      for (dballe::Msgs::const_iterator i = msgs.begin();
           i != msgs.end(); ++i) {
          dump(**i);
      }
      flush();
  }
  void dump(const dballe::Msg& msg) {
      if (opts.ignore_empty && is_empty_message(msg)) return;
      for (std::vector<dballe::msg::Context*>::const_iterator i = msg.data.begin();
           i != msg.data.end(); ++i) {
          dump(msg, **i);
      }
  }
  void dump(const dballe::Msg& msg, const dballe::msg::Context& ctx) {
      for (std::vector<wreport::Var*>::const_iterator i = ctx.data.begin();
           i != ctx.data.end(); ++i) {
          if (!opts.station_ctx && ctx.is_station()) continue;
          dump(msg, ctx, **i);
      }
  }

  void dump(const dballe::Msg& msg, const dballe::msg::Context& ctx, const wreport::Var& var) {
      if (opts.skip_invalid) {
          const wreport::Var *attr = var.enqa(WR_VAR(0,33,196));
          if (attr && attr->enqd())
              return;
      }
      json.start_map();
      json.add_string("type");
      json.add_string("Feature");

      json.add_string("geometry");

      json.start_map();
      json.add_string("type");
      json.add_string("Point");
      json.add_string("coordinates");
      json.start_list();
      if (msg.get_longitude_var())
          json.add_number(msg.get_longitude_var()->format());
      else
          json.add_null();
      if (msg.get_latitude_var())
          json.add_number(msg.get_latitude_var()->format());
      else
          json.add_null();
      json.end_list();
      json.end_map();

      json.add_string("properties");
      json.start_map();
      json.add_string("lon");
      json.add_int(msg.get_longitude_var()->enqi());
      json.add_string("lat");
      json.add_int(msg.get_latitude_var()->enqi());
      json.add_string("datetime");
      if (!ctx.is_station()) {
          int date[6];
          char datestr[23];
          msg.datetime().to_array(date);
          sprintf(datestr, "%4d-%02d-%02dT%02d:%02d:%02dZ", date[0], date[1], date[2], date[3], date[4], date[5]);
          json.add_string(datestr);
      } else {
          json.add_null();
      }
      dump(ctx.level);
      dump(ctx.trange);
      json.add_string("network");
      json.add_string(msg.get_rep_memo_var() ? msg.get_rep_memo_var()->enqc() : dballe::Msg::repmemo_from_type(msg.type));
      json.add_string("ident");
      if (msg.get_ident_var() != NULL)
          json.add_string(msg.get_ident_var()->enqc());
      else
          json.add_null();

      if (opts.geohash > 0) {
          char *hash = GEOHASH_encode(msg.get_latitude_var()->enqd(), msg.get_longitude_var()->enqd(), opts.geohash);
          json.add_string("geohash");
          json.add_string(hash);
          delete hash;
      }

      dump(var);

      json.end_map();

      json.end_map();
  }
  void dump(const dballe::Level& level) {
      std::string keys[] = {
          "level",
          "t1", "v1", "t2", "v2"
      };
      if (opts.collapse) {
          json.add_string(keys[0] + "_" + keys[1]);
          dump(level.ltype1);
          json.add_string(keys[0] + "_" + keys[2]);
          dump(level.l1);
          json.add_string(keys[0] + "_" + keys[3]);
          dump(level.ltype2);
          json.add_string(keys[0] + "_" + keys[4]);
          dump(level.l2);
      } else {
          json.add_string(keys[0]);
          json.start_map();
          json.add_string(keys[1]);
          dump(level.ltype1);
          json.add_string(keys[2]);
          dump(level.l1);
          json.add_string(keys[3]);
          dump(level.ltype2);
          json.add_string(keys[4]);
          dump(level.l2);
          json.end_map();
      }
  }
  void dump(const dballe::Trange& trange) {
      std::string keys[] = {
          "trange",
          "pind", "p1", "p2"
      };
      if (opts.collapse) {
          json.add_string(keys[0] + "_" + keys[1]);
          dump(trange.pind);
          json.add_string(keys[0] + "_" + keys[2]);
          dump(trange.p1);
          json.add_string(keys[0] + "_" + keys[3]);
          dump(trange.p2);
      } else {
          json.add_string(keys[0]);
          json.start_map();
          json.add_string(keys[1]);
          dump(trange.pind);
          json.add_string(keys[2]);
          dump(trange.p1);
          json.add_string(keys[3]);
          dump(trange.p2);
          json.end_map();
      }
  }
  void dump(int i) {
      if (i != dballe::MISSING_INT)
          json.add_int(i);
      else
          json.add_null();
  }
  void dump(const wreport::Var& var) {
      json.add_string("bcode");
      json.add_string(wreport::varcode_format(var.info()->var));
      json.add_string("value");
      if (var.isset()) {
          if (var.info()->is_string() || var.info()->is_binary()) {
              json.add_string(var.format());
          } else {
              json.add_number(var.format());
          }
      } else {
          json.add_null();
      }
      if (opts.attributes) {
          if (!opts.collapse) {
              json.add_string("attr");
              json.start_map();
          }
          for (const wreport::Var* attr = var.next_attr(); attr != NULL; attr = attr->next_attr()) {
              const std::string code = dballe::format_code(attr->code());
              if (!opts.collapse)
                  json.add_string(code);
              else
                  json.add_string(std::string("attr_") + code);
              if (attr->isset())
                  if (attr->info()->is_string() || attr->info()->is_binary())
                      json.add_string(attr->format());
                  else
                      json.add_number(attr->format());
              else
                  json.add_null();
          }
          if (!opts.collapse) {
              json.end_map();
          }
      }
  }
 public:
  GeoJSONDumper(std::ostream &out, DumperOptions &opts) : out(out), opts(opts), json(opts.beautify) {
      json.start_map();
      json.add_string("type");
      json.add_string("FeatureCollection");
      json.add_string("features");
      json.start_list();
  }
  virtual ~GeoJSONDumper() {
      json.end_list();
      json.end_map();
      flush();
  }
  virtual bool operator()(const dballe::cmdline::Item& item) {
      if (item.msgs)
          dump(*(item.msgs));
      flush();
      return true;
  }
};

#include <getopt.h>

void show_help(std::ostream& out) {
    out << "Usage: bufr2json [OPTION]... [FILE]..."  << std::endl
        << "Convert BUFR files to JSON format" << std::endl
        << std::endl
        << " --indent            generate indented output" << std::endl
        << " --include-empty     include empty messages" << std::endl
        << " --no-collapse       do not collapse properties" << std::endl
        << " --station-ctx       print station context" << std::endl
        << " --attributes        print attributes" << std::endl
        << " --geohash[=LENGTH]  print geohash. LENGTH defaults to " << BUFR2JSON_DEFAULT_GEOHASH_SIZE << std::endl
        << " --no-skip-invalid   do not skip invalid data" << std::endl
        << " -h,--help           show this help and exit" << std::endl
        << " -V,--version        show version and exit" << std::endl
        << std::endl
        << "With no FILE, or when FILE is -, read standard input" << std::endl;
}
void show_version(std::ostream& out) {
    out << "bufr2json " << PACKAGE_VERSION << std::endl;
}

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

int main(int argc, char **argv)
{
    struct {
        int indent;
        int ignore_empty;
        int collapse;
        int station_ctx;
        int attributes;
        int geohash;
        int skip_invalid;
    } input_options = {
        false,
        true,
        true,
        false,
        false,
        0,
        true
    };
    std::list<std::string> inputlist;

    while (1) {
        static struct option long_options[] = {
            {"indent",          no_argument,&input_options.indent,       true },
            {"include-empty",   no_argument,&input_options.ignore_empty, false},
            {"no-collapse",     no_argument,&input_options.collapse,     false},
            {"station-ctx",     no_argument,&input_options.station_ctx,  true },
            {"attributes",      no_argument,&input_options.attributes,   true },
            {"geohash",         optional_argument,0,                     'g'  },
            {"no-skip-invalid", no_argument,&input_options.skip_invalid, false},
            {"help",            no_argument,0,                           'h'  },
            {"version",         no_argument,0,                           'V'  },
            {0, 0, 0, 0 }
        };
        int option_index = 0;
        int c = getopt_long(argc, argv, "hV", long_options, &option_index);
        if (c == -1) break;
        switch (c) {
            case 0: break;
            case 'h': show_help(std::cout); return 0;
            case 'V': show_version(std::cout); return 0;
            case 'g': {
                if (optarg == 0)
                    input_options.geohash = BUFR2JSON_DEFAULT_GEOHASH_SIZE;
                else
                    input_options.geohash = ::atoi(optarg);
                break;
            }
            default: show_help(std::cerr); return 1;
        }
    }
    while (optind < argc) {
        std::string f(argv[optind++]);
        if (f == "-") f = "(stdin)";
        inputlist.push_back(f);
    }

    dballe::cmdline::Reader reader;

    DumperOptions opts;
    opts.beautify = ( input_options.indent ? true : false );
    opts.ignore_empty = ( input_options.ignore_empty ? true : false );
    opts.station_ctx = ( input_options.station_ctx ? true : false );
    opts.collapse = ( input_options.collapse ? true : false );
    opts.attributes = ( input_options.attributes ? true : false );
    opts.geohash = input_options.geohash;
    opts.skip_invalid = (input_options.skip_invalid ? true : false );

    GeoJSONDumper dumper(std::cout, opts);

    reader.read(inputlist, dumper);

    return 0;
}
