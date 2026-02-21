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
    // on entry file read pointer is after the 6 byte header
    assert((h.id == 0x003c) || (h.id == 0x0056));
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

        // TODO - do something with the subrecord
        keepGoing = cb.Unsupported(fmt::format("Unknown airport subrecord type {}", srh.id));

        nbytes -= srh.size;
    }

    return keepGoing;
}

}
