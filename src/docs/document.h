/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/logger.h"
#include <vector>
#include <memory>
#include <mupdf/fitz.h>

 // This header file defines the interface for downloaded and local documents,
 // and interfaces for getting the properties of these documents and tile images
 // for display.

namespace navitab {

class RasterTile;

class Document
{
public:
    enum DocStatus {
        OK = 0,
        NOT_FOUND,
        LOAD_TIMEOUT,
        UNSUPPORTED
    };

    Document(const std::string& url, DocStatus err);
    Document(const std::string& url, const std::string& type, std::vector<uint8_t>& data);
    virtual ~Document();

    void Prepare(fz_context* fzc);

    DocStatus Status() { return status;  }

    unsigned PageCount();
    std::pair<unsigned, unsigned> PageSize(unsigned page = 0);

    std::shared_ptr<RasterTile> GetTile(unsigned page, float scaleX, float scaleY, int x, int y, int w = 0, int h = 0);

private:
    void selectPage(int p);
    void dropActivePage();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::string const url;
    DocStatus status;
    std::string const type;
    std::vector<uint8_t> const contents;

    // these are the MuPDF (fitz) references
    fz_context* fzctx;
    fz_stream* stream;
    fz_document* doc;
    int activePageNum;
    fz_display_list* activePageDisplayList;
    int pageCount;
    std::vector<fz_rect> pageRects;

};

} // namespace navitab
