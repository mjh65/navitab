/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <fmt/core.h>
#include "readerapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

ReaderApp::ReaderApp(std::shared_ptr<AppServices> core)
:   App("rdrapp", core)
{
    activeToolsMask = 
        (1 << ClickableTool::REDUCE) |
        (1 << ClickableTool::MAGNIFY) |
        (1 << ClickableTool::LAST) |
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT) |
        (1 << ClickableTool::FIRST) |
        (1 << ClickableTool::ROTATEC) |
        (1 << ClickableTool::ROTATEA) |
        (1 << ClickableTool::BOTTOM) |
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP) |
        (1 << ClickableTool::TOP);
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

void ReaderApp::Assemble()
{
    lv_obj_t * label = lv_label_create(root);
    lv_label_set_text(label, "Document Reader\nTBD");
    lv_obj_center(label);
}

void ReaderApp::Demolish()
{
    UNIMPLEMENTED(__func__);
}

void ReaderApp::ToolClick(ClickableTool t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
}

} // namespace navitab
