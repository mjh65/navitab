/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglairport.h"
#include <fmt/core.h>

namespace navitab {

BglAirportReader::BglAirportReader(SceneryReader::Callbacks &handler, std::ifstream &ifs)
:   cb(handler), fp(ifs)
{
}

bool BglAirportReader::Read(BglRecordHeader &h)
{
    bool keepGoing = true;
    if ((h.id != 0x003c) && (h.id != 0x0056)) {
        return cb.Info(fmt::format("Ignoring unknown record type {} in airport section", h.id));
    }
    // on entry file read pointer is after the 6 byte header
    unsigned fixedPartSize = h.id == 0x3c ? 0x38 : 0x44;
    // read the remaining bytes of the fixed part and extract useful data
    std::vector<unsigned char> fixedHdr;
    fixedHdr.resize(fixedPartSize);
    fp.read(reinterpret_cast<char*>(fixedHdr.data() + 6), fixedPartSize - 6);
    // get the additional subrecord counts
    unsigned nRunways = fixedHdr[6];
    unsigned nComs = fixedHdr[7];
    unsigned nStarts = fixedHdr[8];
    double lonx = decodeBglLongitude(*reinterpret_cast<int32_t *>(fixedHdr.data() + 0x0c));
    double laty = decodeBglLatitude(*reinterpret_cast<int32_t *>(fixedHdr.data() + 0x10));
    double alt = m2ft(*reinterpret_cast<int32_t *>(fixedHdr.data() + 0x14)) / 1000;
    std::string icao = decodeIcao(*reinterpret_cast<uint32_t *>(fixedHdr.data() + 0x28) >> 5);
    std::string region = decodeIcao(*reinterpret_cast<uint32_t *>(fixedHdr.data() + 0x2c));
    std::string name;
    std::string country;
    keepGoing = cb.Info(fmt::format("Airport {}", icao));

    unsigned nbytes = h.size - fixedPartSize;
    while (nbytes >= 6) {
        BglRecordHeader srh(fp);
        std::vector<unsigned char> subRecord;
        subRecord.resize(srh.size);
        fp.read(reinterpret_cast<char*>(subRecord.data() + 6), srh.size - 6);

        switch (srh.id) {
        case 0x0011: // runway start
        case 0x0012: // com
        case 0x0019: // name
        case 0x001a: // taxiway points
        case 0x001d: // taxiway names
        case 0x0026: // helipad
        case 0x0031: // apron lights
        case 0x0033: // deletions
        case 0x0040: // taxi path (p3d-v4)
        case 0x0042: // sid
        case 0x0048: // star
        case 0x0057: // airport light support (msfs)
        case 0x0058: // unknown
        case 0x0059: // unknown
        case 0x005a: // unknown
        case 0x005b: // unknown
        case 0x0066: // tower obj
        case 0x00ce: // runway (msfs)
        case 0x00d4: // taxi path (msfs)
        case 0x00de: // jetway (msfs)
        case 0x00e7: // taxi parking (msfs)
        case 0x00e8: // airport project mesh (msfs)
        case 0x00cf: // painted line (msfs)
        case 0x00d0: // apron 1st (msfs)
        case 0x00d8: // painted hatched area (msfs)
        case 0x00d9: // taxiway sign (msfs)
        case 0x00dd: // taxiway parking name (msfs)
        case 0x00e9: // ground merging transfer (msfs)
        case 0x00fa: // new approach (msfs)
            // silently ignored for now
            break;
        default:
            keepGoing = cb.Unsupported(fmt::format("Unknown airport subrecord type {}", srh.id));
        }

        nbytes -= srh.size;
    }

    return keepGoing;
}

}
