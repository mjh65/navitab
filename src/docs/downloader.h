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
#include <curl/curl.h>

struct fz_context;

namespace navitab {

class Document;

class Downloader
{
public:
    Downloader(const std::string& url);
    std::shared_ptr<Document> Download(bool& cancel, fz_context* fzc);
    virtual ~Downloader();

private:
    static size_t onData(void* buffer, size_t size, size_t nmemb, void* resPtr);
    static int onProgress(void* client, curl_off_t dlTotal, curl_off_t dlNow, curl_off_t ulTotal, curl_off_t ulNow);

private:
    std::unique_ptr<logging::Logger> LOG;
    CURL* curl = nullptr;
    std::string url;
};

}
