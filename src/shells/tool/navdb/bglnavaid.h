/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilestream.h"

namespace navbuilder {

class BglVorIlsReader {

public:
    BglVorIlsReader(SceneryReader::Callbacks& handler, BglFileStream& fs);
    ~BglVorIlsReader() = default;

    bool ReadNextRecord(size_t& maxbytes);

private:
    SceneryReader::Callbacks& cb;
    BglFileStream& fs;
};


class BglNdbReader {

public:
    BglNdbReader(SceneryReader::Callbacks& handler, BglFileStream& fs);
    ~BglNdbReader() = default;

    bool ReadNextRecord(size_t& maxbytes);

private:
    SceneryReader::Callbacks& cb;
    BglFileStream& fs;
};


}
