/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilestream.h"

namespace navbuilder {

class BglWaypointReader {

public:
    BglWaypointReader(SceneryReader::Callbacks& handler, BglFileStream& fs);
    ~BglWaypointReader() = default;

    bool ReadNextRecord(size_t& maxbytes);

private:
    SceneryReader::Callbacks& cb;
    BglFileStream& fs;
};

}
