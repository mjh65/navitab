/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <fmt/core.h>
#include "routeapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

RouteApp::RouteApp(std::shared_ptr<AppServices> core)
:   App("routeapp", core)
{
    activeToolsMask = 
        (1 << ClickableTool::MENU) |
        (1 << ClickableTool::AFFIRM) |
        (1 << ClickableTool::STOP) |
        (1 << ClickableTool::CANCEL) |
        (1 << ClickableTool::LAST) |
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT) |
        (1 << ClickableTool::FIRST);
    repeatingToolsMask = 
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT);
}

void RouteApp::Assemble()
{
    lv_obj_t * label = lv_label_create(root);
    lv_label_set_text(label, "Routing\nTBD");
    lv_obj_center(label);
}

void RouteApp::Demolish()
{
    UNIMPLEMENTED(__func__);
}

void RouteApp::ToolClick(ClickableTool t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
}

} // namespace navitab
