#include <bufr2json/json.h>

#include <wreport/varinfo.h>

#include <yajl/yajl_version.h>

namespace bufr2json {
namespace json {

YajlGenerator::YajlGenerator(bool beautify, const char* indent) {
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
YajlGenerator::~YajlGenerator() {
    yajl_gen_free(json);
}
std::string YajlGenerator::flush() {
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
void YajlGenerator::start_list() {
    yajl_gen_array_open(json);
}
void YajlGenerator::end_list() {
    yajl_gen_array_close(json);
}
void YajlGenerator::start_map() {
    yajl_gen_map_open(json);
}
void YajlGenerator::end_map() {
    yajl_gen_map_close(json);
}
void YajlGenerator::add_null() {
    yajl_gen_null(json);
}
void YajlGenerator::add_bool(bool val) {
    yajl_gen_bool(json, val);
}
void YajlGenerator::add_int(long int val) {
    yajl_gen_integer(json, val);
}
void YajlGenerator::add_double(double val) {
    yajl_gen_double(json, val);
}
void YajlGenerator::add_string(const std::string& val) {
    yajl_gen_string(json, (const unsigned char*)val.c_str(), val.size());
}
void YajlGenerator::add_number(const std::string& val) {
    yajl_gen_number(json, val.c_str(), val.size());
}
void YajlGenerator::add_int_or_null(long int val) {
    if (val == dballe::MISSING_INT)
        add_null();
    else
        add_int(val);
}
void YajlGenerator::add_var_value(const wreport::Var& var) {
    if (var.isset()) {
        if (var.info()->type == wreport::Vartype::String ||
            var.info()->type == wreport::Vartype::Binary) {
            add_string(var.format());
        } else {
            add_number(var.format());
        }
    } else {
        add_null();
    }
}

}
}
