/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "bglnavdata.h"
#include <filesystem>
#include <list>

namespace navbuilder {

class SceneryReader {

public:
    struct Callbacks {
        virtual bool Error(std::string e) = 0;
        virtual bool Warning(std::string w) = 0;
        virtual bool Info(std::string i) = 0;
        virtual bool Unimplemented(std::string e) = 0;
        virtual bool CheckContinue() = 0;

        virtual bool StartingFile(std::filesystem::path f) = 0;
        virtual bool AddNavItem(std::shared_ptr<navitab::navdata::NavItem> ni) = 0;
        virtual bool DoneFile() = 0;

        virtual unsigned NextUid() = 0;

        virtual std::shared_ptr<Airport> GetAirport(unsigned uid) = 0;
        virtual std::shared_ptr<Airport> FindAirport(std::string icao) = 0;
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
