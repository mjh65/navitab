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
    RunLater([this]() { onResize(Width(), Height()); });
}

void CoreKeypad::HideKeypad()
{
    visible = false;
    RunLater([this]() { Redraw(); });
}

void CoreKeypad::onResize(int w, int h)
{
    SetImage(w, h);
    // TODO - generate the basic keypad image, depending on dimensions

    if (!visible) return;

    RunLater([this]() { Redraw(); });
}

void CoreKeypad::onMouseEvent(int x, int y, bool l)
{
    UNIMPLEMENTED(__func__);
}

} // namespace navitab
