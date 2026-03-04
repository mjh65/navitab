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
    cb(handler),
    fs(fname)
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
    uint32_t qmids[8];
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

    if (!fs.good()) {
        return cb.Warning(fmt::format("Failed to open {} for reading", fname.string()));
    }

    BglHeader hdr = {};
    if (!fs.read(&hdr, sizeof(hdr))) {
        return cb.Warning(fmt::format("Failed to read header from {}", fname.string()));
    }

    switch (hdr.magic) {
    case 0x19920201:
        if (hdr.headerSize != 56) {
            return cb.Warning(fmt::format("Unknown header size {}", hdr.headerSize));
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
        return cb.Warning(fmt::format("Unknown header magic {}", hdr.magic));
    }

    if (hdr.sectionCount > 32) {
        return cb.Warning(fmt::format("Too many sections in this file {}", hdr.sectionCount));
    }

    auto numSections = hdr.sectionCount;
    cb.Info(fmt::format("{} sections", numSections));
    std::vector<SectionHeader> sections;
    sections.resize(numSections);
    if (!fs.read(sections.data(), numSections * sizeof(SectionHeader))) {
        return cb.Warning(fmt::format("Failed to read section headers from {}", fname.string()));
    }

    for (int s = 0; keepGoing && (s < numSections); ++s) {
        auto& sptr = sections[s];
        auto sshs = ((sptr.subSectionSizeCoeff & 0x10000) | 0x40000) >> 0x0E;
        if (sptr.subSectionHeaderSize != (sshs * sptr.subSectionCount)) {
            return cb.Warning(fmt::format("Mismatch between number of subsections {} and subsection header size {}",
                                          sptr.subSectionCount, sptr.subSectionHeaderSize));
        }
        cb.Info(fmt::format("Section {} type {} contains {} sub-sections", s, sptr.sectionType, sptr.subSectionCount));
        fs.seek(sptr.fileOffset);
        keepGoing = DoSection(sptr.sectionType, sptr.subSectionCount, sshs);
    }
    
    return keepGoing;
}

bool BglFileReader::DoSection(uint32_t stype, unsigned nss, size_t sshs)
{
    bool keepGoing = true;

    std::vector<unsigned char> subSections;
    subSections.resize(nss * sshs);
    unsigned char *ssp = subSections.data();
    fs.read(subSections.data(), nss * sshs);
    
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
            keepGoing = cb.Info(fmt::format("Ignored terrain section type {}", stype));
            continue;
        }

        fs.seek(fo);
        switch (stype) {
            case 0x0003: // airport data
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoRecords<BglAirportReader>(nr, sr);
                break;
            case 0x0013: // VOR / ILS
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoRecords<BglVorIlsReader>(nr, sr);
                break;
            case 0x0017: // NDB
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoRecords<BglNdbReader>(nr, sr);
                break;
            case 0x0022: // waypoint
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoRecords<BglWaypointReader>(nr, sr);
                break;
            case 0x0027: // namelist
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoRecords<BglNamelistsReader>(nr, sr);
                break;
            case 0x002c: // additional airport data
                assert(sshs == sizeof(SubSection16Header));
                keepGoing = DoRecords<BglAirportSummaryReader>(nr, sr); (nr, sr);
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
                keepGoing = cb.Warning(fmt::format("Unknown section type {}", stype));
                break;
        }
    }
    
    return keepGoing;
}

template<class T>
bool BglFileReader::DoRecords(unsigned nrecords, size_t rsize)
{
    bool keepGoing = true;
    while (keepGoing && nrecords && (rsize >= 6)) {
        T a(cb, fs);
        keepGoing = a.ReadNextRecord(rsize);
        --nrecords;
    }
    return keepGoing;
}

}
