/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"

namespace navitab {

class BglFileReader {

public:
    BglFileReader(std::filesystem::path f, SceneryReader::Callbacks &handler);
    ~BglFileReader();

    bool DoScan();

private:
    std::filesystem::path fname;
    SceneryReader::Callbacks &cb;

};

}
