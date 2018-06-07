/*
 * Copyright (C) 2015  ARPA-SIM <urpsim@smr.arpa.emr.it>
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
 * Author: Emanuele Di Giacomo <edigiacomo@arpae.it>
 */
#ifndef BUFR2JSON_JSON_H
#define BUFR2JSON_JSON_H

#include <string>

#include <yajl/yajl_gen.h>

#include <wreport/var.h>
#include <dballe/types.h>


namespace bufr2json {
namespace json {

/**
 * Simple C++ wrapper for yajl_gen
 */
class YajlGenerator {
 private:
    yajl_gen json;

 public:
    YajlGenerator(bool beautify, const char* indent=NULL);
    ~YajlGenerator();
    std::string flush();
    void start_list();
    void end_list();
    void start_map();
    void end_map();
    void add_null();
    void add_bool(bool val);
    void add_int(long int val);
    void add_double(double val);
    void add_string(const std::string& val);
    void add_number(const std::string& val);
    void add_int_or_null(long int val);
    void add_var_value(const wreport::Var& var);
};

}
}

#endif
