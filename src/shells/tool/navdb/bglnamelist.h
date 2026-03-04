/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilestream.h"

namespace navbuilder {

class BglNamelistsReader {

public:
    BglNamelistsReader(SceneryReader::Callbacks& handler, BglFileStream& fs);
    ~BglNamelistsReader() = default;

    bool ReadNextRecord(size_t& maxbytes);

private:
    SceneryReader::Callbacks& cb;
    BglFileStream& fs;
};

}
