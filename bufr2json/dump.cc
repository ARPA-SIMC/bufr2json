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
#include <bufr2json/dump.h>

#include <sstream>

#include <wreport/var.h>

#include <dballe/message.h>
#include <dballe/msg/msg.h>
#include <dballe/msg/context.h>

namespace bufr2json {
namespace dump {

static void iter_messages(const dballe::Messages& msgs, std::function<void(const dballe::Msg&)> cb) {
    for (dballe::Messages::const_iterator i = msgs.begin(); i != msgs.end(); ++i)
        cb(dballe::Msg::downcast(*i));
}

static void iter_message(const dballe::Msg& msg, std::function<void(const dballe::msg::Context&)> cb) {
    for (std::vector<dballe::msg::Context*>::const_iterator i = msg.data.begin(); i != msg.data.end(); ++i)
        cb(**i);
}

static void iter_context(const dballe::msg::Context& ctx, std::function<void(const wreport::Var&)> cb) {
    for (std::vector<wreport::Var*>::const_iterator i = ctx.data.begin(); i != ctx.data.end(); ++i)
        cb(**i);
}

static void iter_var(const wreport::Var& var, std::function<void(const wreport::Var&)> cb) {
    for (const wreport::Var* attr = var.next_attr(); attr != NULL; attr = attr->next_attr())
        cb(*attr);
}

JsonDumper::JsonDumper(std::ostream& out) : out(out), json(false, NULL) {}

void JsonDumper::flush() {
    out << json.flush();
}

void GeoJsonDumper::start_dump() {
    json.start_map();
    json.add_string("type");
    json.add_string("FeatureCollection");
    json.add_string("features");
    json.start_list();
}

void GeoJsonDumper::end_dump() {
    json.end_list();
    json.end_map();
    flush();
}

bool GeoJsonDumper::operator()(const dballe::cmdline::Item& item) {
    iter_messages(*(item.msgs), [&](const dballe::Msg& msg) {
        iter_message(msg, [&](const dballe::msg::Context& ctx) {
            if (ctx.is_station())
                return;
            iter_context(ctx, [&](const wreport::Var& var) {
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
                std::stringstream ss;
                msg.get_datetime().to_stream_iso8601(ss, 'T', "Z");
                json.add_string(ss.str().c_str());
                json.add_string("level_t1");
                json.add_int_or_null(ctx.level.ltype1);
                json.add_string("level_v1");
                json.add_int_or_null(ctx.level.l1);
                json.add_string("level_t2");
                json.add_int_or_null(ctx.level.ltype2);
                json.add_string("level_v2");
                json.add_int_or_null(ctx.level.l2);
                json.add_string("trange_pind");
                json.add_int_or_null(ctx.trange.pind);
                json.add_string("trange_p1");
                json.add_int_or_null(ctx.trange.p1);
                json.add_string("trange_p2");
                json.add_int_or_null(ctx.trange.p2);
                json.add_string("network");
                json.add_string(msg.get_rep_memo_var() ? msg.get_rep_memo_var()->enqc() : dballe::Msg::repmemo_from_type(msg.type));
                json.add_string("ident");
                if (msg.get_ident_var() != NULL)
                    json.add_string(msg.get_ident_var()->enqc());
                else
                    json.add_null();
                json.add_string("bcode");
                json.add_string(wreport::varcode_format(var.code()));
                json.add_string("value");
                json.add_var_value(var);
                json.end_map();
                json.end_map();
            });
        });
    });
    return true;
}

void DballeJsonDumper::start_dump() {
    json.start_list();
}

void DballeJsonDumper::end_dump() {
    json.end_list();
}

bool DballeJsonDumper::operator()(const dballe::cmdline::Item& item) {
    iter_messages(*(item.msgs), [&](const dballe::Msg& msg) {
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
        iter_message(msg, [&](const dballe::msg::Context& ctx) {
            json.start_map();
            if (not ctx.is_station()) {
                json.add_string("timerange");
                json.start_list();
                json.add_int_or_null(ctx.trange.pind);
                json.add_int_or_null(ctx.trange.p1);
                json.add_int_or_null(ctx.trange.p2);
                json.end_list();
                json.add_string("level");
                json.start_list();
                json.add_int_or_null(ctx.level.ltype1);
                json.add_int_or_null(ctx.level.l1);
                json.add_int_or_null(ctx.level.ltype2);
                json.add_int_or_null(ctx.level.l2);
                json.end_list();
            }
            json.add_string("vars");
            json.start_map();
            iter_context(ctx, [&](const wreport::Var& var) {
                json.add_string(wreport::varcode_format(var.code()));
                json.start_map();
                json.add_string("v");
                json.add_var_value(var);
                json.add_string("a");
                json.start_map();
                iter_var(var, [&](const wreport::Var& attr) {
                    json.add_string(wreport::varcode_format(attr.code()));
                    json.add_var_value(attr);
                });
                json.end_map();
                json.end_map();
            });
            json.end_map();
            json.end_map();
        });
        json.end_list();
        json.end_map();
    });
    return true;
}

}
}
