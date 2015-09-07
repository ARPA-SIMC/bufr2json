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

}
}

#endif
