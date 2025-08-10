/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
