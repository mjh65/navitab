/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglfilereader.h"
#include "bglairport.h"
#include "bglnamelist.h"
#include "bglnavaid.h"
#include "bglwaypoint.h"
#include <fmt/core.h>
#include <cassert>

namespace navbuilder {

BglFileReader::BglFileReader(std::filesystem::path f, SceneryReader::Callbacks &handler)
:   fname(f),
    cb(handler)
{
}

BglFileReader::~BglFileReader()
{
}

struct BglHeader {
    uint32_t magic;
    uint32_t headerSize;
    uint32_t dateTimeLo;
    uint32_t dateTimeHi;
    uint32_t magic2;
    uint32_t sectionCount;
};

struct SectionHeader {
    uint32_t sectionType;
    uint32_t subSectionSizeCoeff;
    uint32_t subSectionCount;
    uint32_t fileOffset;
    uint32_t subSectionHeaderSize;
};

struct SubSection16Header {
    uint32_t qmid;
    uint32_t recordCount;
    uint32_t fileOffset;
    uint32_t recordsSize;
};

struct SubSection20Header {
    uint32_t qmidA;
    uint32_t qmidB;
    uint32_t recordCount;
    uint32_t fileOffset;
    uint32_t recordsSize;
};

bool BglFileReader::DoScan()
{
    bool keepGoing = true;

    fp.open(fname, std::ios::binary);
    if (fp.fail()) {
        return cb.Warning(fmt::format("Cannot open for reading"));
    }

    BglHeader hdr;
    fp.seekg(0, std::ios::beg);
    fp.read(reinterpret_cast<char*>(&hdr), sizeof(BglHeader));

    switch (hdr.magic) {
    case 0x19920201:
        if (hdr.headerSize != 56) {
            return cb.Unsupported(fmt::format("Unknown header size {}", hdr.headerSize));
        }
        break;

    case 0x00000001:
    case 0x9d560001:
        // these BGL files have non-standard magic in their headers
        // 
        // fs-base/scenery/Base/scenery/magdec.bgl 01 00 00 00
        // fs-base-ai-traffic/scenery/world/traffic/trafficAircraft.bgl 01 00 56 9d
        // fs-base-ai-traffic/scenery/world/traffic/trafficBoats.bgl 01 00 56 9d
        // LNM does some special parsing of the magdec.bgl file
        // not sure about the other 2 traffic files

        return cb.Info(fmt::format("Ignoring BGL file {} with non-standard header", fname.string()));
    default:
        return cb.Unsupported(fmt::format("Unknown header magic {}", hdr.magic));
    }

    if (hdr.sectionCount > 32) {
        return cb.Warning(fmt::format("Too many sections in this file {}", hdr.sectionCount));
    }
    auto numSections = hdr.sectionCount;
    cb.Info(fmt::format("{} sections", numSections));
    std::vector<SectionHeader> sections;
    sections.resize(numSections);
    fp.seekg(hdr.headerSize, std::ios::beg);
    fp.read(reinterpret_cast<char*>(sections.data()), numSections * sizeof(SectionHeader));

    for (int s = 0; keepGoing && (s < numSections); ++s) {
        auto& sptr = sections[s];
        auto sshs = ((sptr.subSectionSizeCoeff & 0x10000) | 0x40000) >> 0x0E;
        if (sptr.subSectionHeaderSize != (sshs * sptr.subSectionCount)) {
            return cb.Warning(fmt::format("Mismatch between number of subsections {} and subsection header size {}",
                                          sptr.subSectionCount, sptr.subSectionHeaderSize));
        }
        cb.Info(fmt::format("Section {} type {} contains {} sub-sections", s, sptr.sectionType, sptr.subSectionCount));
        keepGoing = DoSection(sptr.sectionType, sptr.subSectionCount, sshs, sptr.fileOffset);
    }
    
    return keepGoing;
}

bool BglFileReader::DoSection(uint32_t stype, uint32_t nss, uint32_t sshs, uint32_t foffset)
{
    bool keepGoing = true;

    std::vector<unsigned char> subSections;
    subSections.resize(nss * sshs);
    unsigned char *ssp = subSections.data();
    fp.seekg(foffset, std::ios::beg);
    fp.read(reinterpret_cast<char*>(ssp), nss * sshs);
    
    for (int ssi = 0; ssi < nss; ++ssi, ssp += sshs) {
        uint32_t fo, nr, sr;
        if (sshs == sizeof(SubSection16Header)) {
            SubSection16Header *ss16h = reinterpret_cast<SubSection16Header *>(ssp);
            fo = ss16h->fileOffset;
            sr = ss16h->recordsSize;
            nr = ss16h->recordCount;
        } else {
            SubSection20Header *ss20h = reinterpret_cast<SubSection20Header *>(ssp);
            fo = ss20h->fileOffset;
            sr = ss20h->recordsSize;
            nr = ss20h->recordCount;
        }
        cb.Info(fmt::format("Sub-section has {} records {} bytes at offset {}", nr, sr, fo));
        if ((stype >= 0x0065) && (stype <= 0x0098)) {
            // these are all terrain-related sections, not of interest for NAV data
            keepGoing = cb.Info(fmt::format("Ignored section type {}", stype));
            continue;
        }
        switch (stype) {
            case 0x0003: // airport data
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoAirportRecords(fo, sr, nr);
                break;
            case 0x002c: // additional airport data
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoAirportSummaryRecords(fo, sr, nr);
                break;
            case 0x0027: // namelist
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoNamelistsRecords(fo, sr, nr);
                break;
            case 0x0013: // VOR / ILS
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoVorIlsRecords(fo, sr, nr);
                break;
            case 0x0017: // NDB
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoNdbRecords(fo, sr, nr);
                break;
            case 0x0022: // waypoint
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoWaypointRecords(fo, sr, nr);
                break;
                // we don't need anything from these (at the moment!)
            case 0x0018: // markers (inner,middle,outer)
            case 0x0020: // airspace boundary
            case 0x0025: // scenery object
            case 0x0028: // P3D indices
            case 0x0029: // P3D indices
            case 0x002a: // P3D indices
            case 0x002b: // model data
            case 0x002e: // exclusion rectangle
            case 0x002f: // timezone
            case 0x0030: // unknown
            case 0x0031: // P3D indices
                keepGoing = cb.Info(fmt::format("Ignored section type {}", stype));
                break;
            default:
                keepGoing = cb.Unsupported(fmt::format("Unknown section type {}", stype));
                break;
        }
    }
    
    return keepGoing;
}

bool BglFileReader::DoAirportRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords)
{
    bool keepGoing = true;
    while (keepGoing && nrecords) {
        fp.seekg(foffset, std::ios::beg);

        BglRecordHeader h(fp);
        keepGoing = cb.Info(fmt::format("Airport record type {} size {}", h.id, h.size));
        assert(h.size <= rsize);
        
        BglAirportReader a(cb, fp);
        keepGoing = a.Read(h);
        
        foffset += h.size;
        rsize -= h.size;
        --nrecords;
    }
    return keepGoing;
}

bool BglFileReader::DoAirportSummaryRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords)
{
    while (nrecords) {
        fp.seekg(foffset, std::ios::beg);

        uint16_t recordId = 0;
        fp.read(reinterpret_cast<char*>(&recordId), sizeof(uint16_t));
        uint32_t recordSize;
        fp.read(reinterpret_cast<char*>(&recordSize), sizeof(uint32_t));
        cb.Info(fmt::format("Airport summary record type {} size {}", recordId, recordSize));
        assert(recordSize <= rsize);
        
        
        
        foffset += recordSize;
        rsize -= recordSize;
        --nrecords;
    }
    
    return true;
}

bool BglFileReader::DoNamelistsRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords)
{
    while (nrecords) {
        fp.seekg(foffset, std::ios::beg);

        uint16_t recordId = 0;
        fp.read(reinterpret_cast<char*>(&recordId), sizeof(uint16_t));
        uint32_t recordSize;
        fp.read(reinterpret_cast<char*>(&recordSize), sizeof(uint32_t));
        cb.Info(fmt::format("Namelists record type {} size {}", recordId, recordSize));
        assert(recordSize <= rsize);
        
        
        
        foffset += recordSize;
        rsize -= recordSize;
        --nrecords;
    }
    
    return true;
}

bool BglFileReader::DoVorIlsRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords)
{
    while (nrecords) {
        fp.seekg(foffset, std::ios::beg);

        uint16_t recordId = 0;
        fp.read(reinterpret_cast<char*>(&recordId), sizeof(uint16_t));
        uint32_t recordSize;
        fp.read(reinterpret_cast<char*>(&recordSize), sizeof(uint32_t));
        cb.Info(fmt::format("VOR/ILS record type {} size {}", recordId, recordSize));
        assert(recordSize <= rsize);
        
        
        
        foffset += recordSize;
        rsize -= recordSize;
        --nrecords;
    }
    
    return true;
}

bool BglFileReader::DoNdbRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords)
{
    while (nrecords) {
        fp.seekg(foffset, std::ios::beg);

        uint16_t recordId = 0;
        fp.read(reinterpret_cast<char*>(&recordId), sizeof(uint16_t));
        uint32_t recordSize;
        fp.read(reinterpret_cast<char*>(&recordSize), sizeof(uint32_t));
        cb.Info(fmt::format("NDB record type {} size {}", recordId, recordSize));
        assert(recordSize <= rsize);
        
        
        
        foffset += recordSize;
        rsize -= recordSize;
        --nrecords;
    }
    
    return true;
}

bool BglFileReader::DoWaypointRecords(uint32_t foffset, uint32_t rsize, uint32_t nrecords)
{
    while (nrecords) {
        fp.seekg(foffset, std::ios::beg);

        uint16_t recordId = 0;
        fp.read(reinterpret_cast<char*>(&recordId), sizeof(uint16_t));
        uint32_t recordSize;
        fp.read(reinterpret_cast<char*>(&recordSize), sizeof(uint32_t));
        cb.Info(fmt::format("Waypoint record type {} size {}", recordId, recordSize));
        assert(recordSize <= rsize);
        
        
        
        foffset += recordSize;
        rsize -= recordSize;
        --nrecords;
    }
    
    return true;
}

BglRecordHeader::BglRecordHeader(std::ifstream &fp)
{
    fp.read(reinterpret_cast<char*>(&id), sizeof(uint16_t));
    fp.read(reinterpret_cast<char*>(&size), sizeof(uint32_t));
}

}
