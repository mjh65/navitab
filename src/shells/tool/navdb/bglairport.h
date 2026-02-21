/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilereader.h"
#include <fstream>

namespace navitab {

class BglAirportReader {

public:
    BglAirportReader(SceneryReader::Callbacks &handler, std::ifstream &fp);
    ~BglAirportReader() = default;

    bool Read(BglRecordHeader &h);

private:
    SceneryReader::Callbacks &cb;
    std::ifstream &fp;

};

}
