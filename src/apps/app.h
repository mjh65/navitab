/* This file is part of the Navitab project. See the README and LICENSE for details. */

#pragma once

#include <memory>
#include <lvgl.h>
#include "navitab/logger.h"
#include "navitab/toolbar.h"

namespace lvglkit {
class Display;
}

namespace navitab {

struct AppServices;
struct SimStateData;

class App
{
public:
    App(const char *name, std::shared_ptr<AppServices> core);
    virtual ~App();

    void Activate(std::shared_ptr<lvglkit::Display> display);
    void Deactivate();
    
    virtual void FlightLoop(const SimStateData& data) { }
    virtual void ToolClick(ClickableTool t) = 0;

protected:
    virtual void Assemble() = 0;
    virtual void Demolish() = 0;

    // switch the LVGL screen to make it active
    void Show();

protected:
    std::unique_ptr<logging::Logger> LOG;
    std::shared_ptr<AppServices> core;
    std::shared_ptr<lvglkit::Display> display;

    // every app will have an LVGL screen as its root widget
    lv_obj_t * root;

    // every app will have a set of tools that it uses, some of
    // which will automatically repeat post if they are 'held down'
    int activeToolsMask;
    int repeatingToolsMask;
};

} // namespace navitab
