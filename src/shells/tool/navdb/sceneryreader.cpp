/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "sceneryreader.h"
#include "bglfilereader.h"

namespace fs = std::filesystem;

namespace navbuilder {

SceneryReader::SceneryReader(Provider p, Callbacks &handler)
:   sim(p),
    cb(handler),
    keepGoing(true)
{
}

SceneryReader::~SceneryReader()
{
}

int SceneryReader::AddRootPath(std::filesystem::path r)
{
    if (!fs::exists(r) || !fs::is_directory(r)) {
        return 0;
    }
    int nfiles = 0;
    for (const auto& entry : fs::recursive_directory_iterator(r)) {
        if (entry.is_regular_file()) {
            auto ext = entry.path().extension().string();
            if (ext == ".bgl" || ext == ".BGL") {
                files.push_back(entry.path());
                ++nfiles;
            }
        }
    }
    return nfiles;
}

int SceneryReader::DoScan()
{
    while (keepGoing && !files.empty()) {
        auto f = files.front();
        files.pop_front();
        keepGoing = cb.StartingFile(f);
        if (keepGoing) {
            // create a BglFileScanner object and start it scanning
            BglFileReader r(f, cb);
            keepGoing = r.DoScan();
            keepGoing &= cb.DoneFile();
        }
    }
    return 0;
}

}
