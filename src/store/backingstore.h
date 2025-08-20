/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/logger.h"

// This header file defines the interface for the cache database which
// manages the SQLite database that is used for persistent caching of
// documents and map tiles, as well as for storing icon bitmaps that
// are used in the map overlays.

struct sqlite3;

namespace navitab {

struct PathServices;
class ImageBuffer;

class BackingStore
{
public:
    BackingStore(std::shared_ptr<PathServices>);
    virtual ~BackingStore();

    std::shared_ptr<ImageBuffer> GetPixmap(const std::string &name);
    void StorePixmap(const std::string &name, std::shared_ptr<ImageBuffer>);

    int ExecCallback(int n, char **data, char **names);

    protected:
    void CreateTables();

private:
    std::unique_ptr<logging::Logger> LOG;
    sqlite3 *dbHandle;
};

} // namespace navitab
