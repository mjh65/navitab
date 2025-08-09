/*
 *  Navitab - Navigation tablet for VR flight simulation
 *  Copyright (C) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (C) 2018-2024 Folke Will <folko@solhost.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "navitab/logger.h"
#include "navitab/deferred.h"
#include "../imgkit/rasterizer.h"
#include <memory>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <map>
#include <mupdf/fitz.h>


// This header file defines the interface for the document provider which
// manages local and downloaded documents and rendering to raster tiles.
// The document provider implements a cache of downloaded documents and these
// are periodically refreshed. The document provider is used by the maps provider
// to obtain tile documents from the selected slippy tile server.

namespace navitab {

class RasterTile;
class Document;

class DocumentManager
{
public:
    DocumentManager();

    std::shared_ptr<Document> GetDocument(std::string url);

    void MaintenanceTick();

    virtual ~DocumentManager();

protected:
    void AsyncWorker();
    std::shared_ptr<Document> Download(const std::string& url);
    std::shared_ptr<Document> Readfile(const std::string& fpath);

private:
    std::unique_ptr<logging::Logger>    LOG;

    // simple in-memory cache of documents, keyed by URL
    std::map<std::string, std::shared_ptr<Document> > docCache;
    std::mutex                          cacheMutex;

    // flags and state for the asynchronous downloader
    bool cancelDownload;
    bool running;
    std::unique_ptr<std::thread>    worker;
    std::string                     job;
    std::condition_variable         jsync;
    std::mutex                      jmutex;

    fz_context* fzctx;

};

} // namespace navitab
