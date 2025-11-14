/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglfilereader.h"
#include <fmt/core.h>
#include <fstream>

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
    uint16_t fileType;
    uint16_t version;
    uint32_t headerSize;
    uint32_t pad[3];
    uint32_t sectionCount;
};

struct SectionPointer {
    uint32_t sectionType;
    uint32_t pad;
    uint32_t subsectionCount;
    uint32_t fileOffset;
    uint32_t headerSize;
};

bool BglFileReader::DoScan()
{
    std::ifstream f(fname, std::ios::binary);
    if (!f) {
        return cb.Warning(fmt::format("Cannot open for reading"));
    }

    BglHeader hdr;
    f.seekg(0, std::ios::beg);
    f.read(reinterpret_cast<char*>(&hdr), sizeof(BglHeader));

    switch (hdr.fileType) {
    case 0x0201:
        if (hdr.version != 0x1992) {
            return cb.Warning(fmt::format("Unknown header version {}", hdr.version));
        }
        if (hdr.headerSize != 56) {
            return cb.Warning(fmt::format("Unknown header size {}", hdr.headerSize));
        }
        break;
    default:
        return cb.Warning(fmt::format("Unknown header magic {}", hdr.fileType));
    }

    if (hdr.sectionCount > 32) {
        return cb.Warning(fmt::format("Too many sections in this file {}", hdr.sectionCount));
    }
    auto numSections = hdr.sectionCount;
    std::vector<SectionPointer> sections;
    sections.resize(numSections);
    f.seekg(hdr.headerSize, std::ios::beg);
    f.read(reinterpret_cast<char*>(sections.data()), numSections * sizeof(SectionPointer));

    for (int s=0; s<numSections; ++s) {
        auto& sptr = sections[s];

        switch (sptr.sectionType) {
                // we probably need to process these
        case 0x0003: // airport data
        case 0x0013: // VOR / ILS
        case 0x0017: // NDB
        case 0x0020: // airspace boundary (low-priority)
        case 0x0022: // waypoint
        case 0x002c: // additional airport data
        case 0x0027: // namelist
            break;
                // we can probably ignore these
        case 0x0018: // markers (inner,middle,outer)
        case 0x0025: // scenery object
        case 0x002e: // exclusion rectangle
        case 0x0028: // unknown
        case 0x0029: // unknown
        case 0x002a: // unknown
        case 0x0031: // unknown
        case 0x0065: // unknown
            break;
        default:
            bool skip = cb.Warning(fmt::format("Unknown section type {}", sptr.sectionType));
        }

    }
    
    return true; // keep going
}

}
