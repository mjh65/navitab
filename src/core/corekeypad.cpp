/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "corekeypad.h"
#include "navitab.h"

namespace navitab {

CoreKeypad::CoreKeypad(std::shared_ptr<Keypad2Core> c)
:   core(c),
    LOG(std::make_unique<logging::Logger>("keypad")),
    visible(false)
{
}

CoreKeypad::~CoreKeypad()
{
}

void CoreKeypad::ShowKeypad()
{
    visible = true;
    RunLater([this]() { onResize(width, height); });
}

void CoreKeypad::HideKeypad()
{
    visible = false;
    image.reset();
    RunLater([this]() { Redraw(); });
}

void CoreKeypad::onResize(int w, int h)
{
    width = w; height = h;
    if (!visible) return;

    image = std::make_unique<FrameBuffer>(width, height);
    // TODO - generate the basic keypad image, depending on dimensions

    RunLater([this]() { Redraw(); });
}

void CoreKeypad::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
