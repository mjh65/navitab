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

#include "docmanager.h"
#include "downloader.h"
#include "document.h"
#include <fmt/core.h>

namespace navitab {

DocumentManager::DocumentManager()
:   LOG(std::make_unique<logging::Logger>("docmgr")),
    running(true),
    cancelDownload(false),
    fzctx(nullptr)
{
    fzctx = fz_new_context(nullptr, nullptr, FZ_STORE_UNLIMITED);
    if (!fzctx) {
        throw std::runtime_error("Couldn't initialize MuPDF rasterizing libraries");
    }
    fz_try(fzctx) {
        fz_register_document_handlers(fzctx);
    } fz_catch(fzctx) {
        fz_drop_context(fzctx);
        throw std::runtime_error(fmt::format("Cannot register MuPDF document handlers: {}", std::string(fz_caught_message(fzctx))));
    }

    // Start the background worker thread. This thread is used to download documents
    // in the background and put them into the cache. For simplicity only one download
    // can be requested at a time (maybe subject to future enhancement).

    worker = std::make_unique<std::thread>([this]() { AsyncWorker(); });
}

DocumentManager::~DocumentManager()
{
    cancelDownload = true;
    running = false;
    jsync.notify_one();
    worker->join();

    // empty the document cache manually before shutting down MuPDF
    // TODO - do we need to do SQL stuff here? hopefully the maintenance tick has already
    // cached anything we didn't already have?

    docCache.clear();
    fz_drop_context(fzctx);
}

void DocumentManager::MaintenanceTick()
{
    // TODO - do some SQL database stuff here to create a persistent
    // cache between runs.

#if 0 // TODO - rethink this, probably based on number of docs and age
    std::unique_lock<std::mutex> lock(cacheMutex);
    auto ci = docCache.begin();
    while (ci != docCache.end()) {
        auto uc = ci->second.use_count();
        // remove from the cache if not being used elsewhere (TODO, and has been stored in the persistent SQL database)
        if (uc < 2) {
            docCache.erase(ci++);
        } else {
            ++ci;
        }
    }
#endif
}

std::shared_ptr<Document> DocumentManager::GetDocument(std::string url)
{
    // the document is immediately available if it's in the cache
    {
        std::unique_lock<std::mutex> lock(cacheMutex);
        auto ci = docCache.find(url);
        if (ci != docCache.end()) {
            auto& doc = ci->second;
            doc->Prepare(fzctx);
            return doc;
        }
    }

    // can we fetch it from the persistent SQL database?
    // TODO ...

    // otherwise start a job to initiate the fetch in the background if
    // there isn't a job currently in progress
    bool busy = true;
    {
        std::lock_guard<std::mutex> lock(jmutex);
        busy = !job.empty();
    }
    if (!busy) {
        job = url;
        jsync.notify_one();
    }

    // always return nullptr - the requestor will ask again later
    return nullptr;
}

void DocumentManager::AsyncWorker()
{
    while (1) {
        // pause until there's something to do
        std::unique_lock<std::mutex> lock(jmutex);
        jsync.wait(lock, [this]() { return !running || job.size(); });
        if (!running) break;
        lock.unlock();

        // what's required to be done is coded in the job string
        std::shared_ptr<Document> doc = nullptr;
        if (job.substr(0, 5) == "file:") {
            doc = Readfile(job);
        } else {
            doc = Download(job);
        }

        // if the outcome of the work was a document, then put it into the cache
        if (doc) {
            LOGI(fmt::format("Cached {}", job));
            std::unique_lock<std::mutex> lock(cacheMutex);
            docCache[job] = doc;
        }

        // clear the job ready for another request
        lock.lock();
        job = "";
    }
}

std::shared_ptr<Document> DocumentManager::Download(const std::string& url)
{
    Downloader d(url);
    return d.Download(cancelDownload, fzctx);
}

std::shared_ptr<Document> DocumentManager::Readfile(const std::string& fpath)
{
    UNIMPLEMENTED(__func__);
    return nullptr;
}



}

