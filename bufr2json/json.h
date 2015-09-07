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
