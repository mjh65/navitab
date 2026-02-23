/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <memory>
#include <iostream>
#include "sceneryreader.h"

using namespace navitab;

class Builder : public SceneryReader::Callbacks
{
public:
    Builder();
    ~Builder();

    bool Unsupported(std::string u) override;
    bool Error(std::string e) override;
    bool Warning(std::string w) override;
    bool Info(std::string i) override;

    bool StartingFile(std::filesystem::path f) override;
    bool DoneFile() override;

private:
    std::filesystem::path activeFile;
};

int main(int argc, char** argv)
{
    Builder b;
    auto r = std::make_unique<SceneryReader>(SceneryReader::MSFS, b);

    int nfiles = 0;
    nfiles += r->AddRootPath("F:/MSFS/Official");
    nfiles += r->AddRootPath("F:/MSFS/Community");
    nfiles += r->AddRootPath("/Users/michael/work/navitab/scratch");

    int errors = r->DoScan();



    return 0;
}

Builder::Builder()
{

}

Builder::~Builder()
{

}

bool Builder::Unsupported(std::string u)
{
    std::cout << "Unsupported: " << u << std::endl;
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

bool Builder::StartingFile(std::filesystem::path f)
{
    std::cout << "Scanning: " << f << std::endl;
    activeFile = f;
    return true;
}

bool Builder::DoneFile()
{
    std::cout << "Finished: " << activeFile << std::endl;
    return true;
}
