/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglnavaid.h"
#include <fmt/core.h>

namespace navbuilder {

BglVorIlsReader::BglVorIlsReader(SceneryReader::Callbacks& handler, BglFileStream& ifs)
    : cb(handler), fs(ifs)
{
}

bool BglVorIlsReader::ReadNextRecord(size_t& maxbytes)
{
    BglFileStream::RecordHeader h;
    fs.readHeader(h);
    bool keepGoing = cb.Info(fmt::format("VOR/ILS record type {} size {}", h.id, h.size));
    if (!fs.good() || (h.size > maxbytes)) {
        maxbytes = 0;
        return cb.Warning(fmt::format("File corrupted reading VOR/ILS records, skipping sub-section"));
    }

    // TODO
    fs.skip(h.size - 6);
    maxbytes -= h.size;

    return keepGoing;
}


BglNdbReader::BglNdbReader(SceneryReader::Callbacks& handler, BglFileStream& ifs)
    : cb(handler), fs(ifs)
{
}

bool BglNdbReader::ReadNextRecord(size_t& maxbytes)
{
    BglFileStream::RecordHeader h;
    fs.readHeader(h);
    bool keepGoing = cb.Info(fmt::format("NDB record type {} size {}", h.id, h.size));
    if (!fs.good() || (h.size > maxbytes)) {
        maxbytes = 0;
        return cb.Warning(fmt::format("File corrupted reading NDB records, skipping sub-section"));
    }

    // TODO
    fs.skip(h.size - 6);
    maxbytes -= h.size;

    return keepGoing;
}


}
