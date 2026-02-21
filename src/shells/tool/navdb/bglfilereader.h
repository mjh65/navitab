/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include <fstream>
#include <cassert>

namespace navitab {

class BglFileReader {

public:
    BglFileReader(std::filesystem::path f, SceneryReader::Callbacks &handler);
    ~BglFileReader();

    bool DoScan();

private:
    bool DoSection(uint32_t stype, uint32_t nss, uint32_t sshs, uint32_t foffset);
    bool DoAirportRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords);
    bool DoAirportSummaryRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords);
    bool DoNamelistsRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords);
    bool DoVorIlsRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords);
    bool DoNdbRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords);
    bool DoWaypointRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords);

private:
    std::filesystem::path fname;
    std::ifstream fp;
    SceneryReader::Callbacks &cb;

};

struct BglRecordHeader {
    BglRecordHeader() = delete;
    BglRecordHeader(std::ifstream &fp);
    uint16_t id;
    uint32_t size;
};

inline double decodeBglLongitude(int32_t x)
{
    return ((double)x * (360.0f / (3 * 0x10000000))) - 180.0f;
}

inline double decodeBglLatitude(int32_t y)
{
    return 90.0f - (((double) y) * (180.0f / (2 * 0x10000000)));
}

inline std::string decodeIcao(uint32_t v)
{
    std::string s;
    
    while (v) {
        int c = v % 38;
        v = (v - c) / 38;
        if (c >= 12) {
            s.push_back('A' + c - 12);
        } else if (c > 1) {
            s.push_back('0' + c - 2);
        } else if (c == 0) {
            s.push_back(' ');
        } else {
            assert(0);
        }
    }
    std::reverse(s.begin(), s.end());
    return s;
}

inline double m2ft(double m)
{
    return 3.2808399f * m;
}

}
