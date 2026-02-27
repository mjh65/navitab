/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglairport.h"
#include <fmt/core.h>

namespace navbuilder {

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
    double elev = *reinterpret_cast<int32_t *>(fixedHdr.data() + 0x14) / 1000.0f;
    std::string icao = decodeBglIcao(*reinterpret_cast<uint32_t *>(fixedHdr.data() + 0x28) >> 5);
    std::string region = decodeBglIcao(*reinterpret_cast<uint32_t *>(fixedHdr.data() + 0x2c));
    std::string name;
    std::string country;
    arec = cb.FindAirport(icao);
    if (!arec) {
        arec = std::make_shared<Airport>(cb.NextUid());
        arec->_icao = icao;
        arec->_region = region;
        arec->_lonx = lonx;
        arec->_laty = laty;
        arec->_elevation = elev;
        keepGoing = cb.Info(fmt::format("Airport {}", icao));
    } else {
        keepGoing = cb.Warning(fmt::format("Duplicated airport {}", icao));
    }

    unsigned nbytes = h.size - fixedPartSize;
    while (keepGoing && (nbytes >= 6)) {
        BglRecordHeader srh(fp);
        auto recDataSize = srh.size - 6;
        std::vector<unsigned char> subRecord;

        switch (srh.id) {
        case 0x0011: // start
            keepGoing = ReadStart(recDataSize);
            break;
        case 0x0019: // name
            keepGoing = ReadName(recDataSize);
            break;
        case 0x0026: // helipad
            keepGoing = ReadHelipad(recDataSize);
            break;
        case 0x0033: // deletions
            keepGoing = ReadDeletions(recDataSize);
            break;
        case 0x00ce: // runway (msfs)
            keepGoing = ReadRunway(recDataSize);
            break;
        case 0x0012: // com
        case 0x001a: // taxiway points
        case 0x001d: // taxiway names
        case 0x0031: // apron lights
        case 0x0042: // sid
        case 0x0048: // star
        case 0x00de: // jetway (msfs)
        case 0x00fa: // new approach (msfs)
            keepGoing = cb.Info(fmt::format("Unimplemented airport subrecord type {}", srh.id));
            subRecord.resize(srh.size);
            fp.read(reinterpret_cast<char*>(subRecord.data() + 6), srh.size - 6);
            break;
        case 0x0040: // taxi path (p3d-v4)
        case 0x0057: // airport light support (msfs)
        case 0x0058: // unknown
        case 0x0059: // unknown
        case 0x005a: // unknown
        case 0x005b: // unknown
        case 0x0066: // tower obj
        case 0x00cf: // painted line (msfs)
        case 0x00d0: // apron 1st (msfs)
        case 0x00d4: // taxi path (msfs)
        case 0x00d8: // painted hatched area (msfs)
        case 0x00d9: // taxiway sign (msfs)
        case 0x00dd: // taxiway parking name (msfs)
        case 0x00e7: // taxi parking (msfs)
        case 0x00e8: // airport project mesh (msfs)
        case 0x00e9: // ground merging transfer (msfs)
            // these are all ignored
            keepGoing = cb.CheckContinue();
            subRecord.resize(srh.size);
            fp.read(reinterpret_cast<char*>(subRecord.data() + 6), srh.size - 6);
            break;
        default:
            keepGoing = cb.Unsupported(fmt::format("Unknown airport subrecord type {}", srh.id));
        }

        nbytes -= srh.size;
    }

    if (keepGoing) keepGoing = cb.AddNavItem(arec);
    
    return keepGoing;
}

bool BglAirportReader::ReadName(size_t rs)
{
    std::vector<char> raw;
    raw.resize(rs);
    fp.read(raw.data(), rs);
    arec->_name = raw.data();
    return cb.CheckContinue();
}

bool BglAirportReader::ReadHelipad(size_t rs)
{
    std::vector<char> raw;
    raw.resize(rs);
    fp.read(raw.data(), rs);
    auto hrec = std::make_shared<Helipad>(cb.NextUid());
    hrec->_surface = decodeBglSurface(static_cast<uint8_t>(raw[0]));
    hrec->_lonx = decodeBglLongitude(*reinterpret_cast<int32_t *>(raw.data() + 0x0c - 6));
    hrec->_laty = decodeBglLatitude(*reinterpret_cast<int32_t *>(raw.data() + 0x10 - 6));
    hrec->_elevation = *reinterpret_cast<int32_t *>(raw.data() + 0x14 - 6) / 1000.0f;
    hrec->_heading = *reinterpret_cast<float *>(raw.data() + 0x18 - 6);
    arec->_helipads.push_back(hrec);
    return cb.CheckContinue();
}

bool BglAirportReader::ReadStart(size_t rs)
{
    std::vector<char> raw;
    raw.resize(rs);
    fp.read(raw.data(), rs);
    auto srec = std::make_shared<Start>(cb.NextUid());
    uint8_t rn = static_cast<uint8_t>(raw[0]);
    uint8_t rd = static_cast<uint8_t>(raw[1]);
    srec->_type = decodeBglStartType(rd);
    srec->_name = decodeBglRunwayName(rn, rd);
    srec->_lonx = decodeBglLongitude(*reinterpret_cast<int32_t *>(raw.data() + 0x08 - 6));
    srec->_laty = decodeBglLatitude(*reinterpret_cast<int32_t *>(raw.data() + 0x0c - 6));
    srec->_elevation = *reinterpret_cast<int32_t *>(raw.data() + 0x10 - 6) / 1000.0f;
    srec->_heading = *reinterpret_cast<float *>(raw.data() + 0x14 - 6);
    arec->_starts.push_back(srec);
    return cb.CheckContinue();
}

bool BglAirportReader::ReadDeletions(size_t rs)
{
    std::vector<char> raw;
    raw.resize(rs);
    fp.read(raw.data(), rs);
    cb.Warning(fmt::format("Unimplemented deletion record for {}", arec->icao()));
    return cb.CheckContinue();
}

bool BglAirportReader::ReadRunway(size_t rs)
{
    std::vector<uint8_t> raw;
    raw.resize(rs);
    fp.read(reinterpret_cast<char*>(raw.data()), rs);

    return cb.CheckContinue();
}


}
