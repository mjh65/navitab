/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include <fstream>
#include <cassert>

namespace navbuilder {

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

inline std::string decodeBglIcao(uint32_t v)
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

inline navitab::navdata::Surface::Type decodeBglSurface(uint8_t s)
{
    switch (s) {
    case 0: return navitab::navdata::Surface::Type::Concrete;
    case 1: return navitab::navdata::Surface::Type::Grass;
    case 2: return navitab::navdata::Surface::Type::Water;
    case 4: return navitab::navdata::Surface::Type::Asphalt;
    case 7: return navitab::navdata::Surface::Type::Clay;
    case 8: return navitab::navdata::Surface::Type::Snow;
    case 9: return navitab::navdata::Surface::Type::Ice;
    case 12: return navitab::navdata::Surface::Type::Dirt;
    case 13: return navitab::navdata::Surface::Type::Coral;
    case 14: return navitab::navdata::Surface::Type::Gravel;
    case 17: return navitab::navdata::Surface::Type::Bituminous;
    case 18: return navitab::navdata::Surface::Type::Brick;
    case 19: return navitab::navdata::Surface::Type::Macadam;
    case 20: return navitab::navdata::Surface::Type::Wood;
    case 21: return navitab::navdata::Surface::Type::Sand;
    case 22: return navitab::navdata::Surface::Type::Shale;
    case 23: return navitab::navdata::Surface::Type::Tarmac;
    default : return navitab::navdata::Surface::Type::Unknown;
    }
}

inline std::string decodeBglRunwayName(uint8_t rn, uint8_t rd)
{
    std::string n;
    if (rn <= 36) {
        n.push_back('0' + (rn / 10));
        n.push_back('0' + (rn % 10));
    } else if (rn <= 44) {
        static const char *cdirs[] = { "n", "ne", "e", "se", "s", "sw", "w", "nw" };
        n = cdirs[rn - 36];
    }
    static const char *rdes = " LRCWAB";
    auto i = rd & 0x0f;
    if ((i >= 1) && (i <= 6)) {
        n.push_back(rdes[i]);
    }
    return n;
}

inline navitab::navdata::Start::Type decodeBglStartType(uint8_t rd)
{
    switch (rd >> 4) {
    case 1: return navitab::navdata::Start::Type::Runway;
    case 2: return navitab::navdata::Start::Type::Water;
    case 3: return navitab::navdata::Start::Type::Helipad;
    default : return navitab::navdata::Start::Type::Unknown;
    }
}

inline double m2ft(double m)
{
    return 3.2808399f * m;
}

}
