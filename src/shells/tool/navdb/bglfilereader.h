/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "sceneryreader.h"
#include <fstream>

namespace navitab {

class BglFileReader {

public:
    BglFileReader(std::filesystem::path f, SceneryReader::Callbacks &handler);
    ~BglFileReader();

    bool DoScan();

private:
    bool DoSection(uint32_t stype, uint32_t nss, uint32_t sshs, uint32_t foffset);

private:
    std::filesystem::path fname;
    std::ifstream fp;
    SceneryReader::Callbacks &cb;

};

}
