/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include <fstream>

namespace navbuilder {

class BglFileStream {

public:
    struct RecordHeader {
        uint16_t id;
        uint32_t size;
    };
public:
    BglFileStream(std::filesystem::path f);
    ~BglFileStream() = default;

    bool good() const { return fp.good(); }

    void seek(size_t pos) { fp.seekg(pos); }
    size_t tell() { return fp.tellg(); }
    void skip(size_t nbytes) { fp.seekg(nbytes, fp.cur); }

    bool read(void* buffer, size_t size);

    template<class T> bool read(T &);

    bool readHeader(RecordHeader& hdr);
    bool readLongitude(double& lonx);
    bool readLatitude(double& laty);
    bool readElevation(float& elev);
    bool readCodedId(std::string& id, bool isRegion = false);
    //bool readCodedSurface(navitab::navdata::Surface::Type& sfc);

protected:
    std::filesystem::path fname;
    std::ifstream fp;

};

#if 0
struct BglRecordHeader {
    BglRecordHeader() = delete;
    BglRecordHeader(BglFileStream& fs);
    uint16_t id;
    uint32_t size;
};
#endif

}
