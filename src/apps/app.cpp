/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "app.h"
#include "../lvglkit/display.h"
#include "navitab/core.h"

namespace navitab {

App::App(const char *name, std::shared_ptr<AppServices> c)
:   LOG(std::make_unique<logging::Logger>(name)),
    core(c),
    root(nullptr),
    activeToolsMask(0),
    repeatingToolsMask(0)
{
}

App::~App()
{
    if (root) {
        lv_obj_del(root);
        root = nullptr;
    }
}

void App::Activate(std::shared_ptr<lvglkit::Display> d)
{
    display = d;
    if (!root) {
        display->Select();
        root = lv_obj_create(nullptr);
        Assemble();
    }
    Show();
}

void App::Deactivate()
{
    Demolish();
}

void App::Show()
{
    display->Select();
    lv_scr_load(root);
    core->EnableTools(activeToolsMask, repeatingToolsMask);
}

} // namespace navitab
