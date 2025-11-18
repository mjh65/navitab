/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglfilereader.h"
#include <fmt/core.h>

namespace navitab {

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
    uint32_t subsectionCount;
    uint32_t fileOffset;
    uint32_t headerSize;
};

struct SubSection20Header {
    uint32_t qmidA;
    uint32_t qmidB;
    uint32_t subsectionCount;
    uint32_t fileOffset;
    uint32_t headerSize;
};

union SubSectionHeader {
    SubSection16Header h16;
    SubSection20Header h20;
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
    default:
        return cb.Unsupported(fmt::format("Unknown header magic {}", hdr.magic));
    }

    if (hdr.sectionCount > 32) {
        return cb.Warning(fmt::format("Too many sections in this file {}", hdr.sectionCount));
    }
    auto numSections = hdr.sectionCount;
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
        keepGoing = DoSection(sptr.sectionType, sptr.subSectionCount, sshs, sptr.fileOffset);
    }
    
    return keepGoing;
}

bool BglFileReader::DoSection(uint32_t stype, uint32_t nss, uint32_t sshs, uint32_t foffset)
{
    bool keepGoing = true;

    std::vector<SubSectionHeader> subSections;
    subSections.resize(nss);
    SubSectionHeader shdr;
    fp.seekg(foffset, std::ios::beg);
    fp.read(reinterpret_cast<char*>(&shdr), sshs);

    switch (stype) {
        // we probably need to process these
        case 0x0003: // airport data
        case 0x0013: // VOR / ILS
        case 0x0017: // NDB
        case 0x0022: // waypoint
        case 0x002c: // additional airport data
        case 0x0027: // namelist
        // we might want to process these
        case 0x0020: // airspace boundary
        // we can probably ignore these
        case 0x0018: // markers (inner,middle,outer)
        case 0x0025: // scenery object
        case 0x002e: // exclusion rectangle
        case 0x0028: // unknown
        case 0x0029: // unknown
        case 0x002a: // unknown
        case 0x0031: // unknown
        case 0x0065: // unknown
        default:
            keepGoing = cb.Unsupported(fmt::format("Unknown section type {}", stype));
    }

    return keepGoing;
}

}
