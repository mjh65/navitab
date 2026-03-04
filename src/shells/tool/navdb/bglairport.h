/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include "bglfilestream.h"

namespace navbuilder {

class BglAirportReader {

public:
    BglAirportReader(SceneryReader::Callbacks &handler, BglFileStream &fs);
    ~BglAirportReader() = default;

    bool ReadNextRecord(size_t &maxbytes);

private:
    bool ReadName(size_t rs);
    bool ReadHelipad(size_t rs);
    bool ReadStart(size_t rs);
    bool ReadDeletions(size_t rs);
    bool ReadRunway(size_t rs);
    bool ReadCom(size_t rs);

private:
    SceneryReader::Callbacks &cb;
    BglFileStream &fs;
    std::shared_ptr<Airport> arec;

};


class BglAirportSummaryReader {

public:
    BglAirportSummaryReader(SceneryReader::Callbacks& handler, BglFileStream& fs);
    ~BglAirportSummaryReader() = default;

    bool ReadNextRecord(size_t& maxbytes);

private:
    SceneryReader::Callbacks& cb;
    BglFileStream& fs;
};

}
