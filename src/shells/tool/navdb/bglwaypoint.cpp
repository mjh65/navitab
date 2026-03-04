/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "bglwaypoint.h"
#include <fmt/core.h>

namespace navbuilder {

BglWaypointReader::BglWaypointReader(SceneryReader::Callbacks& handler, BglFileStream& ifs)
:   cb(handler), fs(ifs)
{
}

bool BglWaypointReader::ReadNextRecord(size_t& maxbytes)
{
    BglFileStream::RecordHeader h;
    fs.readHeader(h);
    bool keepGoing = cb.Info(fmt::format("Waypoint record type {} size {}", h.id, h.size));
    if (!fs.good() || (h.size > maxbytes)) {
        maxbytes = 0;
        return cb.Warning(fmt::format("File corrupted reading waypoint records, skipping sub-section"));
    }

    // TODO
    fs.skip(h.size - 6);
    maxbytes -= h.size;

    return keepGoing;
}

}
