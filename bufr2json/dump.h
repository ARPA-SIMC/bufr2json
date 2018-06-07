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
#ifndef BUFR2JSON_DUMP_H
#define BUFR2JSON_DUMP_H

#include <ostream>

#include <dballe/cmdline/processor.h>

#include <bufr2json/json.h>


namespace bufr2json {
namespace dump {

class JsonDumper : public dballe::cmdline::Action {
 protected:
    std::ostream& out;
    json::YajlGenerator json;

 public:
    JsonDumper(std::ostream& out);
    virtual void start_dump() = 0;
    virtual void end_dump() = 0;
    void flush();
};

class GeoJsonDumper : public JsonDumper {
 public:
    using JsonDumper::JsonDumper;
    virtual void start_dump() override;
    virtual void end_dump() override;
    virtual bool operator()(const dballe::cmdline::Item& item) override;
};

class DballeJsonDumper : public JsonDumper {
 public:
    using JsonDumper::JsonDumper;
    virtual void start_dump() override;
    virtual void end_dump() override;
    virtual bool operator()(const dballe::cmdline::Item& item) override;
};

}
}

#endif
