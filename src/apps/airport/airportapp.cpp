/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <fmt/core.h>
#include "airportapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

AirportApp::AirportApp(std::shared_ptr<AppServices> core)
:   App("aprtapp", core)
{
    activeToolsMask = 
        (1 << ClickableTool::AFFIRM) |
        (1 << ClickableTool::CANCEL) |
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::MAGNIFY) |
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT) |
        (1 << ClickableTool::ROTATEC) |
        (1 << ClickableTool::ROTATEA) |
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP);
    repeatingToolsMask =
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::MAGNIFY) |
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT) |
        (1 << ClickableTool::ROTATEC) |
        (1 << ClickableTool::ROTATEA) |
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP);
}

void AirportApp::Assemble()
{
    lv_obj_t * label = lv_label_create(root);
    lv_label_set_text(label, "Airports\nTBD");
    lv_obj_center(label);
}

void AirportApp::Demolish()
{
    UNIMPLEMENTED(__func__);
}

void AirportApp::ToolClick(ClickableTool t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
}

} // namespace navitab
