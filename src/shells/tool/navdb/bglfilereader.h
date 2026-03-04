/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilestream.h"
#include <fstream>
#include <cassert>

namespace navbuilder {

class BglFileReader {

public:
    BglFileReader(std::filesystem::path f, SceneryReader::Callbacks &handler);
    ~BglFileReader();

    bool DoScan();

private:
    bool DoSection(uint32_t stype, unsigned nss, size_t sshs);

    template<class T>
    bool DoRecords(unsigned nrecords, size_t rsize);

private:
    std::filesystem::path fname;
    SceneryReader::Callbacks &cb;
    BglFileStream fs;

};

inline navitab::navdata::Surface::Type decodeBglSurface(uint16_t s)
{
    switch (s & 0x7f) {
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
    default: return navitab::navdata::Surface::Type::Unknown;
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

inline navitab::navdata::Com::Type decodeBglComType(uint16_t ct)
{
    switch (ct) {
    case 1: return navitab::navdata::Com::Type::ATIS;
    case 2: return navitab::navdata::Com::Type::Multicom;
    case 3: return navitab::navdata::Com::Type::Unicom;
    case 4: return navitab::navdata::Com::Type::CTAF;
    case 5: return navitab::navdata::Com::Type::Ground;
    case 6: return navitab::navdata::Com::Type::Tower;
    case 7: return navitab::navdata::Com::Type::Clearance;
    case 8: return navitab::navdata::Com::Type::Approach;
    case 9: return navitab::navdata::Com::Type::Departure;
    case 10: return navitab::navdata::Com::Type::Centre;
    case 11: return navitab::navdata::Com::Type::FSS;
    case 12: return navitab::navdata::Com::Type::AWOS;
    case 13: return navitab::navdata::Com::Type::ASOS;
    case 14: return navitab::navdata::Com::Type::Clearance;
    case 15: return navitab::navdata::Com::Type::Delivery;
    default: return navitab::navdata::Com::Type::Unknown;
    }
}

inline double m2ft(double m)
{
    return 3.2808399f * m;
}

}
