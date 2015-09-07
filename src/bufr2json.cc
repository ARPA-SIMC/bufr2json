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

#include <geohash.h>

#include <bufr2json/json.h>

#define BUFR2JSON_DEFAULT_GEOHASH_SIZE 12

using bufr2json::json::YajlGenerator;

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
  void dump(const dballe::Messages& msgs) {
      for (dballe::Messages::const_iterator i = msgs.begin();
           i != msgs.end(); ++i) {
          dump(dballe::Msg::downcast(*i));
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
          std::stringstream ss;
          msg.get_datetime().to_stream_iso8601(ss, 'T', "Z");
          json.add_string(ss.str().c_str());
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
      json.add_string(wreport::varcode_format(var.code()));
      json.add_string("value");
      if (var.isset()) {
          if (var.info()->type == wreport::Vartype::String ||
              var.info()->type == wreport::Vartype::Binary) {
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
              char code[6] = "";
              dballe::format_code(attr->code(), code);
              if (!opts.collapse)
                  json.add_string(code);
              else
                  json.add_string(std::string("attr_") + std::string(code));
              if (attr->isset())
                  if (attr->info()->type == wreport::Vartype::String ||
                      attr->info()->type == wreport::Vartype::Binary)
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

struct DballeJSONDumper : public dballe::cmdline::Action {
  private:
    std::ostream& out;
    DumperOptions opts;
    YajlGenerator json;

    void dump(int i) {
        if (i != dballe::MISSING_INT)
            json.add_int(i);
        else
            json.add_null();
    }

    void dump(const dballe::msg::Context& ctx) {
        json.start_map();
        if (not ctx.is_station()) {
            json.add_string("timerange");
            json.start_list();
            dump(ctx.trange.pind);
            dump(ctx.trange.p1);
            dump(ctx.trange.p2);
            json.end_list();
            json.add_string("level");
            json.start_list();
            dump(ctx.level.ltype1);
            dump(ctx.level.l1);
            dump(ctx.level.ltype2);
            dump(ctx.level.l2);
            json.end_list();
        }
        json.add_string("vars");
        json.start_map();
        for (std::vector<wreport::Var*>::const_iterator i = ctx.data.begin(); i != ctx.data.end(); ++i) {
            const wreport::Var& var = **i;
            json.add_string(wreport::varcode_format(var.code()));
            json.start_map();
            json.add_string("v");
            if (var.isset()) {
                if (var.info()->type == wreport::Vartype::String ||
                        var.info()->type == wreport::Vartype::Binary) {
                    json.add_string(var.format());
                } else {
                    json.add_number(var.format());
                }
            } else {
                json.add_null();
            }
            json.add_string("a");
            json.start_map();
            for (const wreport::Var* attr = var.next_attr(); attr != NULL; attr = attr->next_attr()) {
                json.add_string(wreport::varcode_format(attr->code()));
                if (attr->isset()) {
                    if (attr->info()->type == wreport::Vartype::String ||
                            attr->info()->type == wreport::Vartype::Binary) {
                        json.add_string(attr->format());
                    } else {
                        json.add_number(attr->format());
                    }
                } else {
                    json.add_null();
                }
            }
            json.end_map();
            json.end_map();
        }
        json.end_map();
        json.end_map();
    }

    void dump(const dballe::Msg& msg) {
        json.start_map();
        json.add_string("network");
        json.add_string(msg.get_rep_memo_var() ? msg.get_rep_memo_var()->enqc() : dballe::Msg::repmemo_from_type(msg.type));
        json.add_string("ident");
        if (msg.get_ident_var() != NULL)
            json.add_string(msg.get_ident_var()->enqc());
        else
            json.add_null();
        json.add_string("lon");
        json.add_int(msg.get_longitude_var()->enqi());
        json.add_string("lat");
        json.add_int(msg.get_latitude_var()->enqi());
        json.add_string("date");
        std::stringstream ss;
        msg.get_datetime().to_stream_iso8601(ss, 'T', "Z");
        json.add_string(ss.str().c_str());
        json.add_string("data");
        json.start_list();
        for (std::vector<dballe::msg::Context*>::const_iterator i = msg.data.begin(); i != msg.data.end(); ++i) {
            dump(**i);
            flush();
        }
        json.end_list();
        json.end_map();
        flush();
    }
    void dump(const dballe::Messages& msgs) {
        for (dballe::Messages::const_iterator i = msgs.begin(); i != msgs.end(); ++i)
            dump(dballe::Msg::downcast(*i));
    }

  public:
    DballeJSONDumper(std::ostream &out, DumperOptions &opts) : out(out), opts(opts), json(opts.beautify) {
        json.start_list();
    }
    ~DballeJSONDumper() {
        json.end_list();
        flush();
    }

    void flush() {
        out << json.flush();
    }

    virtual bool operator()(const dballe::cmdline::Item& item) {
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
        << " -f,--format=FORMAT     JSON output format (geojson, dballe. Default: geojson)" << std::endl
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
        if (f == "-") f = "(stdin)";
        inputlist.push_back(f);
    }

    dballe::cmdline::Reader reader;

    DumperOptions opts;
    opts.beautify = false;
    opts.ignore_empty = true;
    opts.station_ctx = false;
    opts.collapse = true;
    opts.attributes = false;
    opts.geohash = false;
    opts.skip_invalid = false;

    std::unique_ptr<dballe::cmdline::Action> action;
    if (input_options.format == "geojson") {
        action.reset(new GeoJSONDumper(std::cout, opts));
    } else if (input_options.format == "dballe") {
        action.reset(new DballeJSONDumper(std::cout, opts));
    } else {
        std::cerr << "Invalid JSON format: '" << input_options.format << "'" << std::endl;
        return 1;
    }

    reader.read(inputlist, *action);

    return 0;
}
