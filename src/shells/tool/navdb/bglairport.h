/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilereader.h"
#include <fstream>

namespace navbuilder {



class BglAirportReader {

public:
    BglAirportReader(SceneryReader::Callbacks &handler, std::ifstream &fp);
    ~BglAirportReader() = default;

    bool Read(BglRecordHeader &h);

private:
    bool ReadName(size_t rs);
    bool ReadHelipad(size_t rs);
    bool ReadStart(size_t rs);
    bool ReadDeletions(size_t rs);
    bool ReadRunway(size_t rs);

private:
    SceneryReader::Callbacks &cb;
    std::ifstream &fp;
    std::shared_ptr<Airport> arec;

};

}
