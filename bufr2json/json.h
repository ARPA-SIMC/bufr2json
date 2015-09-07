#ifndef BUFR2JSON_JSON_H
#define BUFR2JSON_JSON_H

namespace bufr2json {
namespace json {

#include <yajl/yajl_gen.h>

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

}
}

#endif
