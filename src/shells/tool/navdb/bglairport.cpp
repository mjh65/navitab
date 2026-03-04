/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglairport.h"
#include "bglfilereader.h"
#include <fmt/core.h>
#include <algorithm>

namespace navbuilder {

BglAirportReader::BglAirportReader(SceneryReader::Callbacks& handler, BglFileStream& ifs)
    : cb(handler), fs(ifs)
{
}

bool BglAirportReader::ReadNextRecord(size_t& maxbytes)
{
    auto rstartpos = fs.tell();
    BglFileStream::RecordHeader h;
    fs.readHeader(h);
    bool keepGoing = cb.Info(fmt::format("Airport record type {} size {}", h.id, h.size));
    if (!fs.good() || (h.size > maxbytes)) {
        maxbytes = 0;
        return cb.Warning(fmt::format("File corrupted reading airport records, skipping sub-section"));
    }

    if ((h.id != 0x003c) && (h.id != 0x0056)) {
        fs.skip(h.size - 6);
        maxbytes -= h.size;
        return cb.Info(fmt::format("Ignoring unknown record type {} in airport section", h.id));
    }

    // read from the fixed sized header
    unsigned fixedPartSize = h.id == 0x3c ? 0x38 : 0x44;
    uint8_t nRunway, nComm, nStart, nApproach, nApron, nHelipad;
    fs.read(nRunway);
    fs.read(nComm);
    fs.read(nStart);
    fs.read(nApproach);
    fs.read(nApron);
    fs.read(nHelipad);
    double lonx, laty;
    float elev;
    fs.readLongitude(lonx);
    fs.readLatitude(laty);
    fs.readElevation(elev);
    fs.skip(16); // tower location and magvar
    std::string icao, region;
    fs.readCodedId(icao);
    fs.readCodedId(region, true);

    maxbytes -= fixedPartSize;
    fs.seek(rstartpos + fixedPartSize);

    arec = cb.FindAirport(icao);
    if (!arec) {
        arec = std::make_shared<Airport>(cb.NextUid());
        keepGoing = cb.Info(fmt::format("Airport {}", icao));
    }
    else {
        keepGoing = cb.Warning(fmt::format("Duplicated airport {}", icao));
    }
    arec->_icao = icao;
    arec->_region = region;
    arec->_lonx = lonx;
    arec->_laty = laty;
    arec->_elevation = elev;

    // now read any subrecords that are bundled after the airport fixed part
    while (keepGoing && (maxbytes >= 6)) {
        BglFileStream::RecordHeader srh;
        fs.readHeader(srh);
        auto recDataSize = srh.size - 6;

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
            keepGoing = ReadCom(recDataSize);
            break;
        case 0x0042: // sid
        case 0x0048: // star
        case 0x00de: // jetway (msfs)
        case 0x00fa: // new approach (msfs)
            keepGoing = cb.Unimplemented(fmt::format("no handler for airport subrecord type {}", srh.id));
            fs.skip(srh.size - 6);
            break;

            // these are all deliberately ignored
        case 0x001a: // taxiway points
        case 0x001d: // taxiway names
        case 0x0031: // apron lights
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
            keepGoing = cb.CheckContinue();
            fs.skip(srh.size - 6);
            break;
        default:
            keepGoing = cb.Warning(fmt::format("Unknown airport subrecord type {}", srh.id));
            fs.skip(srh.size - 6);
        }

        maxbytes -= srh.size;
    }

    if (keepGoing) keepGoing = cb.AddNavItem(arec);

    return keepGoing;
}

bool BglAirportReader::ReadName(size_t rs)
{
    std::vector<char> raw;
    raw.resize(rs);
    fs.read(raw.data(), rs);
    arec->_name = raw.data();
    return cb.CheckContinue();
}

bool BglAirportReader::ReadHelipad(size_t rs)
{
    if (rs != (0x24 - 6)) {
        fs.skip(rs);
        return cb.Warning(fmt::format("Unknown format of helipad record for {}", arec->icao()));
    }
    auto hrec = std::make_shared<Helipad>(cb.NextUid());
    uint8_t sc;
    fs.read(sc);
    hrec->_surface = decodeBglSurface(sc);
    fs.skip(5);
    fs.readLongitude(hrec->_lonx);
    fs.readLatitude(hrec->_laty);
    fs.readElevation(hrec->_elevation);
    fs.skip(8);
    fs.read(hrec->_heading);
    arec->_helipads.push_back(hrec);
    return cb.CheckContinue();
}

bool BglAirportReader::ReadStart(size_t rs)
{
    if (rs != (0x18 - 6)) {
        fs.skip(rs);
        return cb.Warning(fmt::format("Unknown format of start record for {}", arec->icao()));
    }
    auto srec = std::make_shared<Start>(cb.NextUid());
    uint8_t rn, rd;
    fs.read(rn); fs.read(rd);
    srec->_type = decodeBglStartType(rd);
    srec->_name = decodeBglRunwayName(rn, rd);
    fs.readLongitude(srec->_lonx);
    fs.readLatitude(srec->_laty);
    fs.readElevation(srec->_elevation);
    fs.read(srec->_heading);
    arec->_starts.push_back(srec);
    return cb.CheckContinue();
}

bool BglAirportReader::ReadDeletions(size_t rs)
{
    fs.skip(rs);
    cb.Unimplemented(fmt::format("Unimplemented deletion record for {}", arec->icao()));
    return cb.CheckContinue();
}

bool BglAirportReader::ReadRunway(size_t rs)
{
    const size_t kRunwayRecordSize = 0x34 + 44; // 0x34 according to fsdeveloper wiki, plus 44 additional according to atools
    if (rs < (kRunwayRecordSize - 6)) {
        fs.skip(rs);
        return cb.Warning(fmt::format("Unknown format of runway record for {}", arec->icao()));
    }

    auto rwyp = std::make_shared<Runway>(cb.NextUid(), arec.get());
    auto rwys = std::make_shared<Runway>(cb.NextUid(), arec.get());

    uint16_t cs;
    fs.read(cs);
    rwyp->_surface = rwys->_surface = decodeBglSurface(cs);
    uint8_t rn, rd;
    fs.read(rn); fs.read(rd);
    rwyp->_name = decodeBglRunwayName(rn, rd);
    fs.read(rn); fs.read(rd);
    rwys->_name = decodeBglRunwayName(rn, rd);
    fs.readCodedId(rwyp->_ilsIcao);
    fs.readCodedId(rwys->_ilsIcao);
    fs.readLongitude(rwyp->_lonx); rwys->_lonx = rwyp->lonx();
    fs.readLatitude(rwyp->_laty); rwys->_laty = rwyp->laty();
    fs.readElevation(rwyp->_elevation); rwys->_elevation = rwyp->elevation();
    fs.read(rwyp->_length); rwys->_length = rwyp->length();
    fs.read(rwyp->_width); rwys->_width = rwyp->width();
    fs.read(rwyp->_heading); rwys->_heading = (rwyp->heading() >= 180.0f) ? rwyp->heading() - 180.0f : rwyp->heading() + 180.0f;
    // skip the pattern, marking and light fields
    fs.skip(8);
    // skip extra stuff not documented in the fsdeveloper wiki, but handled in atools library
    // this includes runways surface guids, which we currently ignore
    fs.skip(44);

    // scan sub-sub-records - we only use the threshold offset
    size_t rssrsize = rs - (kRunwayRecordSize - 6);
    while (fs.good() && (rssrsize >= 6)) {
        BglFileStream::RecordHeader ssrh;
        fs.readHeader(ssrh);
        size_t ssrs = std::min((size_t)ssrh.size, rssrsize);
        rssrsize -= ssrs;
        if ((ssrh.id == 0x0005) || (ssrh.id == 0x0006)) {
            fs.skip(2);
            float threshold;
            if (ssrh.id == 0x0005) { fs.read(rwyp->_threshold); } else { fs.read(rwys->_threshold); }
            fs.skip(4);
        } else {
            fs.skip(ssrs - 6);
        }
    }

    // register primary runway, and secondary runway if it is valid
    arec->_runways.push_back(rwyp);
    if (rwys->name().size()) {
        rwyp->_pairid = rwys->_pairid;
        rwys->_pairid = rwyp->_pairid;
        arec->_runways.push_back(rwys);
    }

    return cb.CheckContinue();
}

bool BglAirportReader::ReadCom(size_t rs)
{
    if (rs < 8) {
        fs.skip(rs);
        return cb.Warning(fmt::format("Unknown format of com record for {}", arec->icao()));
    }
    auto crec = std::make_shared<Com>(cb.NextUid());
    uint16_t ct;
    fs.read(ct);
    crec->_type = decodeBglComType(ct);
    uint32_t fq;
    fs.read(fq);
    crec->_freq = (float)fq / 1000000.0f;
    std::vector<char> raw;
    raw.resize(rs - 6);
    fs.read(raw.data(), rs - 6);
    crec->_name = raw.data();
    if (crec->freq()) {
        arec->_coms.push_back(crec);
    }
    return cb.CheckContinue();
}




BglAirportSummaryReader::BglAirportSummaryReader(SceneryReader::Callbacks& handler, BglFileStream& ifs)
    : cb(handler), fs(ifs)
{
}

bool BglAirportSummaryReader::ReadNextRecord(size_t& maxbytes)
{
    BglFileStream::RecordHeader h;
    fs.readHeader(h);
    bool keepGoing = cb.Info(fmt::format("Airport summary record type {} size {}", h.id, h.size));
    if (!fs.good() || (h.size > maxbytes)) {
        maxbytes = 0;
        return cb.Warning(fmt::format("File corrupted reading airport records, skipping sub-section"));
    }

    // TODO
    fs.skip(h.size - 6);
    maxbytes -= h.size;

    return keepGoing;
}

}