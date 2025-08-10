/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "downloader.h"
#include "document.h"
#include "navitab/config.h"
#include <fmt/core.h>

namespace navitab {

Downloader::Downloader(const std::string& u)
:   LOG(std::make_unique<logging::Logger>("dwnldr")),
    curl(curl_easy_init()),
    url(u)
{
}

Downloader::~Downloader()
{
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

std::shared_ptr<Document> Downloader::Download(bool& cancel, fz_context* fzc)
{
    if (!curl) {
        LOGE("Unable to initialise curl for document download");
        return nullptr;
    }

    std::vector<uint8_t> downloadBuf;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Navitab " NAVITAB_VERSION_STR);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, onProgress);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &cancel);

    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&downloadBuf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, onData);

    CURLcode code = curl_easy_perform(curl);

    if (code != CURLE_OK) {
        if (code == CURLE_ABORTED_BY_CALLBACK) {
            return nullptr;
        } else {
            LOGE(fmt::format("Error {} downloading {}", curl_easy_strerror(code), url));
            return std::make_shared<Document>(url, Document::DocStatus::NOT_FOUND);
        }
    }

    long httpStatus = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
    if (httpStatus != 200) {
        LOGE(fmt::format("Error status {} downloading {}", httpStatus, url));
        return std::make_shared<Document>(url, Document::DocStatus::NOT_FOUND);
    }

    // get the document type
    char* ct = nullptr;
    curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);

    return std::make_shared<Document>(url, ct, downloadBuf);
}

size_t Downloader::onData(void* buffer, size_t size, size_t nmemb, void* vecPtr)
{
    std::vector<uint8_t>* vec = reinterpret_cast<std::vector<uint8_t> *>(vecPtr);
    if (!vec) {
        return 0;
    }
    size_t pos = vec->size();
    vec->resize(pos + size * nmemb);
    std::memcpy(vec->data() + pos, buffer, size * nmemb);
    return size * nmemb;
}

int Downloader::onProgress(void* client, curl_off_t dlTotal, curl_off_t dlNow, curl_off_t ulTotal, curl_off_t ulNow)
{
    bool* cancel = reinterpret_cast<bool*>(client);
    return *cancel;
}


}
