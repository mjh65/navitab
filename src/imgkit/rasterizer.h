/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include "navitab/window.h"
#include "navitab/logger.h"
#include <memory>
#include <vector>
#include <string>
#include <mupdf/fitz.h>

namespace navitab {

class Rasterizer {
public:
    Rasterizer(const std::string &utf8Path);
    Rasterizer(const std::vector<uint8_t> &data, const std::string type);

    int getTileSize();
    int getPageWidth(int page, int zoom);
    int getPageHeight(int page, int zoom);
    double getAspectRatio(int page);
    std::unique_ptr<ImageBuffer> loadTile(int page, int x, int y, int zoom, bool nightMode);
    void setPreRotate(int angle);

    int getPageCount() const;

    virtual ~Rasterizer();

private:
    std::unique_ptr<logging::Logger> LOG;
    std::vector<uint8_t> dataBuf;
    std::vector<fz_rect> pageRects;
    bool logLoadTimes = false;
    int tileSize = 1024;
    int totalPages = 0;
    int currentPageNum = 0;
    int preRotateAngle = 0;
    fz_stream *stream{};
    fz_document *doc{};
    fz_display_list *currentPageList {};

    void initFitz();
    void loadFile(const std::string &file);
    void loadMemory(const std::vector<uint8_t> &data, const std::string type);
    void loadDocument();
    void loadPage(int page);
    float zoomToScale(int zoom) const;
    void freeCurrentPage();
};

} // namespace navitab

