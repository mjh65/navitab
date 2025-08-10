/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "document.h"
#include "navitab/tiles.h"
#include <fmt/core.h>

namespace navitab {

Document::Document(const std::string& u, DocStatus e)
:   LOG(std::make_unique<logging::Logger>("docmnt")),
    url(u),
    status(e),
    type("UNKNOWN"),
    fzctx(nullptr),
    stream(nullptr),
    doc(nullptr),
    activePageNum(-1),
    activePageDisplayList(nullptr)
{
    // This is the constructor used to create a missing document. Keeping it in the
    // cache will avoid continuous retrying.
}

Document::Document(const std::string& u, const std::string& t, std::vector<uint8_t>& data)
:   LOG(std::make_unique<logging::Logger>("docmnt")),
    url(u),
    status(OK),
    type(t.size() ? t : "application/pdf"),
    contents(data),
    fzctx(nullptr),
    stream(nullptr),
    doc(nullptr),
    activePageNum(-1),
    activePageDisplayList(nullptr)
{
    // This constructor is used for downloaded documents stored in memory.
}

Document::~Document()
{
    dropActivePage();
    if (doc) fz_drop_document(fzctx, doc);
    if (stream) fz_drop_stream(fzctx, stream);
}

void Document::Prepare(fz_context* fzc)
{
    if (fzctx) return; // already prepared
    fzctx = fzc;

    fz_try(fzctx) {
        stream = fz_open_memory(fzctx, contents.data(), contents.size());
        doc = fz_open_document_with_stream(fzctx, type.c_str(), stream);
    } fz_catch(fzctx) {
        if (stream) {
            fz_drop_stream(fzctx, stream);
            stream = nullptr;
        }
        doc = nullptr;
        LOGE(fmt::format("MuPDF could not open {}. It reported {}", url, fz_caught_message(fzctx)));
        status = UNSUPPORTED;
    }

    fz_try(fzctx) {
        pageCount = fz_count_pages(fzctx, doc);
        LOGI(fmt::format("{} has {} pages", url, pageCount));
    } fz_catch(fzctx) {
        LOGE(fmt::format("MuPDF could not get page count for {}. It reported {}", url, fz_caught_message(fzctx)));
        status = UNSUPPORTED;
    }

    for (int i = 0; i < pageCount; ++i) {
        auto page = fz_load_page(fzctx, doc, i);
        if (!page) {
            LOGE(fmt::format("MuPDF could not load page {} for {}. It reported {}", i, url, fz_caught_message(fzctx)));
            status = UNSUPPORTED;
            break;
        }
        auto rect = fz_bound_page(fzctx, page);
        pageRects.push_back(rect);
        fz_drop_page(fzctx, page);
    }
}

void Document::selectPage(int p)
{
    if ((activePageNum == p) && activePageDisplayList) return;

    dropActivePage();

    fz_try(fzctx) {
        activePageDisplayList = fz_new_display_list_from_page_number(fzctx, doc, p);
        activePageNum = p;
    } fz_catch(fzctx) {
        LOGE(fmt::format("MuPDF could not parse page {} for {}. It reported {}", p, url, fz_caught_message(fzctx)));
        activePageDisplayList = nullptr;
        activePageNum = -1;
    }
}

void Document::dropActivePage()
{
    if (activePageNum >= 0) {
        if (activePageDisplayList) {
            fz_drop_display_list(fzctx, activePageDisplayList);
            activePageDisplayList = nullptr;
        }
        activePageNum = -1;
    }
}

std::pair<unsigned, unsigned> Document::PageSize(unsigned page)
{
    auto& rect = pageRects.at(page);
    return std::pair<unsigned, unsigned>(rect.x1 - rect.x0, rect.y1 - rect.y0);
}

std::shared_ptr<RasterTile> Document::GetTile(unsigned page, float scaleX, float scaleY, int x, int y, int w, int h)
{
    selectPage(page);

    if (!w) w = RasterTile::DefaultWidth;
    if (!h) h = RasterTile::DefaultHeight;

    auto tile = std::make_shared<RasterTile>(w, h);

    int outStartX = w * x;
    int outStartY = h * y;

    int outWidth = w;
    int outHeight = h;

    fz_irect clipBox;
    clipBox.x0 = outStartX;
    clipBox.x1 = outStartX + outWidth;
    clipBox.y0 = outStartY;
    clipBox.y1 = outStartY + outHeight;

    fz_pixmap* pix = nullptr;
    fz_try(fzctx) {
        uint8_t* outBuf = (uint8_t*)tile->Row(0);
        pix = fz_new_pixmap_with_data(fzctx, fz_device_rgb(fzctx), outWidth, outHeight, nullptr, 1, outWidth * 4, outBuf);
        pix->x = clipBox.x0;
        pix->y = clipBox.y0;
        pix->xres = 72; // 72 is the normal resolution of MuPDF
        pix->yres = 72;
    } fz_catch(fzctx) {
        LOGE(fmt::format("MuPDF could not create pixmap. It reported {}", fz_caught_message(fzctx)));
        return tile;
    }

    fz_device* dev = nullptr;
    fz_try(fzctx) {
        auto& rect = pageRects.at(activePageNum);
        int currentPageWidth = rect.x1 - rect.x0;
        int currentPageHeight = rect.y1 - rect.y0;

        int translateX = 0, translateY = 0;

        fz_matrix scaleMatrix = fz_scale(scaleX, scaleY);
        fz_matrix rotateMatrix = fz_rotate(0);
        fz_matrix rotateAndScaleMatrix = fz_concat(scaleMatrix, rotateMatrix);
        fz_matrix translateMatrix = fz_translate(translateX, translateY);
        fz_matrix transformMatrix = fz_concat(rotateAndScaleMatrix, translateMatrix);

        dev = fz_new_draw_device_with_bbox(fzctx, transformMatrix, pix, &clipBox);

        // pre-fill page with white
        fz_path* path = fz_new_path(fzctx);
        fz_moveto(fzctx, path, 0, 0);
        fz_lineto(fzctx, path, 0, currentPageHeight);
        fz_lineto(fzctx, path, currentPageWidth, currentPageHeight);
        fz_lineto(fzctx, path, currentPageWidth, 0);
        fz_closepath(fzctx, path);
        float white = 1.0f;
        fz_fill_path(fzctx, dev, path, 0, fz_identity, fz_device_gray(fzctx), &white, 1.0f, fz_default_color_params);
        fz_drop_path(fzctx, path);

        fz_rect pageRect;
        pageRect.x0 = 0;
        pageRect.y0 = 0;
        pageRect.x1 = currentPageWidth;
        pageRect.y1 = currentPageHeight;
        fz_run_display_list(fzctx, activePageDisplayList, dev, fz_identity, pageRect, nullptr);
        fz_close_device(fzctx, dev);
        fz_drop_device(fzctx, dev);
    } fz_catch(fzctx) {
        if (dev) {
            fz_drop_device(fzctx, dev);
        }
        fz_drop_pixmap(fzctx, pix);
        LOGE(fmt::format("MuPDF could not render. It reported {}", fz_caught_message(fzctx)));
    }

    if (pix) {
        fz_drop_pixmap(fzctx, pix);
    }

    return tile;
}


}
