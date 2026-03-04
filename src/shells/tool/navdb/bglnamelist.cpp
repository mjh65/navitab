/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglnamelist.h"
#include <fmt/core.h>

namespace navbuilder {

BglNamelistsReader::BglNamelistsReader(SceneryReader::Callbacks& handler, BglFileStream& ifs)
:   cb(handler), fs(ifs)
{
}

bool BglNamelistsReader::ReadNextRecord(size_t& maxbytes)
{
    BglFileStream::RecordHeader h;
    fs.readHeader(h);
    bool keepGoing = cb.Info(fmt::format("Namelists record type {} size {}", h.id, h.size));
    if (!fs.good() || (h.size > maxbytes)) {
        maxbytes = 0;
        return cb.Warning(fmt::format("File corrupted reading namelists records, skipping sub-section"));
    }

    // TODO
    fs.skip(h.size - 6);
    maxbytes -= h.size;

    return keepGoing;
}

}
