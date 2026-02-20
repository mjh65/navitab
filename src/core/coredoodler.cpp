/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "coredoodler.h"
#include "navitab.h"

namespace navitab {

CoreDoodler::CoreDoodler(std::shared_ptr<Doodler2Core> c)
:   LOG(std::make_unique<logging::Logger>("doodler")),
    core(c),
    enabled(false)
{
}

CoreDoodler::~CoreDoodler()
{
}

void CoreDoodler::EnableDoodler()
{
    if (enabled) return;

    enabled = true;
    RunLater([this]() { onResize(Width(), Height()); });
}

void CoreDoodler::DisableDoodler()
{
    if (!enabled) return;

    enabled = false;
    RunLater([this]() { Redraw(); });
}

void CoreDoodler::onResize(int w, int h)
{
    // if the doodler is resized then a new image is created,
    // and the old image is copied into the new image.

    std::unique_ptr<ImageBuffer> prev;
    Swap(prev); // prev now contains the doodle before resizing
    SetImage(w, h);
    Image()->Clear(backgroundPixels);
    if (prev) {
        for (auto y = 0; y < std::min(h, (int)prev->Height()); ++y) {
            auto sr = Image()->PixAt(y, 0);
            auto dr = prev->PixAt(y, 0);
            auto nx = std::min(w, (int)prev->Width());
            std::copy(sr, sr + nx, dr);
        }
    }
    Invalidate(ImageRegion(0, 0, Width(), Height()));
    RunLater([this]() { Redraw(); });
}

void CoreDoodler::onMouseEvent(int x, int y, bool l)
{
    UNIMPLEMENTED(__func__);
}

void CoreDoodler::onKeyEvent(int c)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
