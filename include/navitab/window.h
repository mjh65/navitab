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

#include <cassert>
#include <cstring>
#include <memory>
#include <algorithm>
#include <functional>
#include <vector>
#include "navitab/deferred.h"


namespace navitab {

struct CoreServices;
class Settings;
struct Window;
struct PartPainter;
struct Toolbar;
struct Modebar;
struct Doodler;
struct Keypad;
class ImageBuffer;
class ImageRegion;

// The PartPainter interface defines the services the UI window provides to the
// window parts. This is currently only a request to paint the window part!

struct PartPainter
{
    // This is called whenever one of the window parts has been updated and needs repainting.
    virtual void Paint(int part, const ImageBuffer* src, const std::vector<ImageRegion>& updates) = 0;
};

// The WindowControls interface defines the services the UI window provides to the
// Navitab core system. This is currently only a request to adjust the brightness.

struct WindowControls
{
    // Adjust the brightness of the display
    virtual void Brightness(int percent) = 0;
};

// The Window interface defines the services that the UI window provides to the
// application/plugin shell. These relate mainly to subsystem connectivity and
// interactions.

struct Window
{
    enum {
        TOOLBAR_HEIGHT = 24,
        TOOL_ICON_WIDTH = 24,
        MODEBAR_WIDTH = 40,
        MODE_ICON_HEIGHT = 40,
        KEYPAD_ICON_HEIGHT = 24,
        MODEBAR_HEIGHT = MODE_ICON_HEIGHT * 7 + KEYPAD_ICON_HEIGHT, // 8 mode selectors, one smaller!
        KEYPAD_HEIGHT = 200,
        WIN_MIN_WIDTH = 400,
        WIN_STD_WIDTH = 800,
        WIN_MAX_WIDTH = 1600,
        WIN_MIN_HEIGHT = TOOLBAR_HEIGHT + MODEBAR_HEIGHT,
        WIN_STD_HEIGHT = 480,
        WIN_MAX_HEIGHT = 1000,
    };

    // ===============================================================
    // Factory function to create a GUI window object. There will be
    // one of these in each of the simulator-specific libraries.
    static std::shared_ptr<Window> Factory();

    // ===============================================================
    // APIs called from the application/plugin

    // Initialisation and shutdown of the window.
    virtual void Connect(std::shared_ptr<CoreServices> core) = 0;
    virtual void Disconnect() = 0;

    // Run the event loop and return when the window is closed.
    virtual void EventLoop() = 0;

    virtual ~Window() = default;
};

// The ImageRegion stucture defines a rectangular area, typically for the purposes
// of updating when only a smaller part of the image has been modified.

struct ImageRegion
{
    int left, top, right, bottom;
    ImageRegion(int l, int t, int r, int b) : left(l), top(t), right(r), bottom(b) {}
    ImageRegion(const ImageRegion& r1, const ImageRegion& r2)
    :   left(std::max(r1.left, r2.left)), top(std::max(r1.top, r2.top)),
        right(std::min(r1.right, r2.right)), bottom(std::min(r1.bottom, r2.bottom)) {}
    bool Empty() const { return (left >= right) || (top >= bottom); }
};

// A PixelBuffer object is a convenience class that collates the storage, width and
// height of a region of memory that holds an image. These objects do not necessarily
// own the buffer, and so are normally only intended for transient use.

class PixelBuffer
{
public:
    PixelBuffer(unsigned w, unsigned h, uint32_t* d) : width(w), height(h), span(w), data(d) { }
    PixelBuffer(unsigned w, unsigned h, unsigned s, uint32_t* d) : width(w), height(h), span(s), data(d) { }
    ~PixelBuffer() = default;

    unsigned Width() const { return width; }
    unsigned Height() const { return height; }

    uint32_t* Row(unsigned r) { return data + (r * span); }
    uint32_t* Pixel(unsigned x, unsigned y) { return data + (y * span) + x; }

    void PaintArea(const ImageRegion& dstArea, PixelBuffer& src, const ImageRegion& srcArea);

protected:
    PixelBuffer(unsigned w, unsigned h) : width(w), height(h), span(w), data(nullptr) { }
    void SetData(uint32_t* d) { data = d; }

protected:
    unsigned width;
    unsigned height;
    unsigned span;
    uint32_t* data;

};

// ImageBuffer objects are PixelBuffers that manage their storage, and are often
// rendered by the Navitab core or apps, and given to the window interface to be
// displayed. The name FrameBuffer is a synonym and is used in some APIs.

class ImageBuffer : public PixelBuffer
{
public:
    ImageBuffer(unsigned w, unsigned h) : PixelBuffer(w, h) { data.resize(width * height); SetData(&data[0]); }
    ~ImageBuffer() = default;

    void Clear(uint32_t px) { std::fill(data.begin(), data.end(), px); }

    void PaintIcon(unsigned x, unsigned y, const uint32_t *pix, unsigned w, unsigned h, uint32_t bg = 0);

    std::vector<uint32_t>::iterator PixAt(unsigned y, unsigned x) { return data.begin() + (y * width + x); }
    std::vector<uint32_t>::const_iterator PixAt(unsigned y, unsigned x) const { return data.begin() + (y * width + x); }
    const std::vector<uint32_t>& Data() const { return data; }

private:
    std::vector<uint32_t> data;
};
typedef class ImageBuffer FrameBuffer; // synonym

// Each window part (toolbar, modebar, canvas, doodler, keypad) implements
// this interface so that the window manager can pass on UI events of interest.

class WindowPart : public DeferredJobRunner<>
{
public:
    enum {
        CANVAS,
        TOOLBAR,
        MODEBAR,
        DOODLER,
        KEYPAD,
        TOTAL_PARTS
    };

    // Set the interface to the painter this window part should work with.
    // This may be called 'mid-flight' as the X-Plane client window will change
    // when switching to/from VR.
    void SetPainter(std::shared_ptr<PartPainter> p) {
        RunLater([this, p]() { onSetPainter(p); });
    }
    void PostResize(int w, int h) {
        RunLater([this, w, h]() { onResize(w, h); });
    }
    void PostMouseEvent(int x, int y, bool l, bool r) {
        RunLater([this, x, y, l, r]() { onMouseEvent(x, y, l, r); });
    }
    void PostWheelEvent(int x, int y, int xdir, int ydir) {
        RunLater([this, x, y, xdir, ydir]() { onWheelEvent(x, y, xdir, ydir); });
    }
    void PostKeyEvent(int code) {
        RunLater([this, code]() { onKeyEvent(code); });
    }
    PixelBuffer GetPixelBuffer() {
        return PixelBuffer(image->Width(), image->Height(), image->Row(0));
    }

protected:
    // Called at start, and then whenever the window part is resized.
    virtual void onResize(int width, int height) { assert(0); }

    // Called when a mouse event occurs. Includes movement while a button is down.
    // Position coordinates are relative to canvas top-left.
    virtual void onMouseEvent(int x, int y, bool l, bool r) { assert(0); }

    // Called when scroll wheel events occur.
    virtual void onWheelEvent(int x, int y, int xdir, int ydir) { assert(0); }

    // Called when key events occur.
    virtual void onKeyEvent(int code) { assert(0); }

protected:
    WindowPart(int id) : partId(id) { }
    ~WindowPart() = default;

    void onSetPainter(std::shared_ptr<PartPainter> p) {
        painter = p;
    }

    void Redraw() {
        if (dirtyBits.empty()) return;
        painter->Paint(partId, image.get(), dirtyBits);
        dirtyBits.clear();
    }

protected:
    int const partId;
    std::shared_ptr<PartPainter> painter;
    std::unique_ptr<ImageBuffer> image;
    int width, height;
    std::vector<ImageRegion> dirtyBits;

};

inline void PixelBuffer::PaintArea(const ImageRegion& dstArea, PixelBuffer& src, const ImageRegion& srcArea)
{
    assert((dstArea.right - dstArea.left) == (srcArea.right - srcArea.left));
    assert((dstArea.bottom - dstArea.top) == (srcArea.bottom - srcArea.top));

    auto rowCount = srcArea.bottom - srcArea.top;
    auto rowSize = srcArea.right - srcArea.left;

    for (unsigned ri = 0; ri < rowCount; ++ri) {
        auto dr = Row(dstArea.top + ri);
        auto sr = src.Row(srcArea.top + ri);
        memcpy(dr + dstArea.left, sr + srcArea.left, rowSize * sizeof(uint32_t));
    }
}

inline void ImageBuffer::PaintIcon(unsigned x, unsigned y, const uint32_t *pix, unsigned w, unsigned h, uint32_t bg)
{
    assert((x + w) <= width);
    assert((y + h) <= height);

    for (int iy = 0; iy < h; ++iy) {
        uint32_t *d = &data[((y + iy) * width) + x];
        const uint32_t *s = pix + iy * w;
        if (bg == 0) {
            memcpy(d, s, w * sizeof(uint32_t));
        } else {
            for (int k = 0; k < w; ++k) {
                uint32_t p = *s++;
                *d++ = (p == 0) ? bg : p;
            }
        }
    }
}


} // namespace navitab
