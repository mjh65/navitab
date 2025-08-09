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
