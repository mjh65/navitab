/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <filesystem>
#include <list>

namespace navitab {

class SceneryReader {

public:
    struct Callbacks {
        virtual bool Unsupported(std::string e) = 0;
        virtual bool Error(std::string e) = 0;
        virtual bool Warning(std::string w) = 0;
        virtual bool StartingFile(std::filesystem::path f) = 0;
        virtual bool DoneFile() = 0;
    };

public:
    enum Provider { MSFS, XPLANE };
    SceneryReader(Provider p, Callbacks &handler);
    ~SceneryReader();

    int AddRootPath(std::filesystem::path r);

    int DoScan();

private:
    Provider const sim;
    Callbacks &cb;
    bool keepGoing;
    std::list<std::filesystem::path> files;
};

}
