/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <memory>
#include <vector>
#include <map>
#include <algorithm>
#include <iostream>
#include <cassert>
#include "sceneryreader.h"

using namespace navbuilder;

class Builder : public SceneryReader::Callbacks
{
public:
    Builder();
    ~Builder();

    bool Error(std::string e) override;
    bool Warning(std::string w) override;
    bool Info(std::string i) override;
    bool Unimplemented(std::string u) override;
    bool CheckContinue() override;

    bool StartingFile(std::filesystem::path f) override;
    bool AddNavItem(std::shared_ptr<navitab::navdata::NavItem> ni) override;
    bool DoneFile() override;

    unsigned NextUid() override;

    std::shared_ptr<Airport> GetAirport(unsigned uid) override;
    std::shared_ptr<Airport> FindAirport(std::string icao) override;

private:
    std::filesystem::path activeFile;
    unsigned nextUid;
    std::vector<std::shared_ptr<Airport> > airports;
    std::map<std::string, std::shared_ptr<Airport> > airportsByIcao;
};

int main(int argc, char** argv)
{
    Builder b;
    auto r = std::make_unique<SceneryReader>(SceneryReader::MSFS, b);

    // TODO - scan following LNM technique (base pkgs then Content.xml)
    int nfiles = 0;
    nfiles += r->AddRootPath("F:/MSFS/Official");
    nfiles += r->AddRootPath("F:/MSFS/Community");
    nfiles += r->AddRootPath("/Users/michael/work/navitab/scratch");

    int errors = r->DoScan();



    return 0;
}

Builder::Builder()
:   nextUid(1)
{

}

Builder::~Builder()
{

}

bool Builder::Unimplemented(std::string u)
{
    std::cout << "U: " << u << std::endl;
    return false;
}

bool Builder::Error(std::string e)
{
    std::cout << "E: " << e << std::endl;
    return false;
}

bool Builder::Warning(std::string w)
{
    std::cout << "W: " << w << std::endl;
    return true;
}

bool Builder::Info(std::string i)
{
    std::cout << "I: " << i << std::endl;
    return true;
}

bool Builder::CheckContinue()
{
    return true;
}

bool Builder::StartingFile(std::filesystem::path f)
{
    std::cout << "Scanning: " << f << std::endl;
    activeFile = f;
    return true;
}

bool Builder::AddNavItem(std::shared_ptr<navitab::navdata::NavItem> ni)
{
    // use std::dynamic_pointer_cast
    auto ap = std::dynamic_pointer_cast<Airport>(ni);
    if (ap) {
        airports.push_back(ap);
        auto icao = ap->icao();
        assert(icao.size());
        airportsByIcao[icao] = ap;
        return true;
    }

    assert(0);
    return false;
}

bool Builder::DoneFile()
{
    std::cout << "Finished: " << activeFile << std::endl;
    return true;
}

unsigned Builder::NextUid()
{
    return nextUid++;
}

std::shared_ptr<Airport> Builder::GetAirport(unsigned uid)
{
    // binary search, since vector will be in ascending uid order
    // use std::lower_bound();
    assert(0);
    return nullptr;
}

std::shared_ptr<Airport> Builder::FindAirport(std::string icao)
{
    auto ai = airportsByIcao.find(icao);
    if (ai != airportsByIcao.end()) {
        return ai->second;
    }
    return nullptr;
}
