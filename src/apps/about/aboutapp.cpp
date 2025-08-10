/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <fmt/core.h>
#include "aboutapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

AboutApp::AboutApp(std::shared_ptr<AppServices> core)
:   App("about", core)
{
    activeToolsMask = 
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP);
    repeatingToolsMask = 
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP);
}

void AboutApp::Assemble()
{
    lv_obj_t * label = lv_label_create(root);
    lv_label_set_text(label, "About Navitab\nTBD");
    lv_obj_center(label);
}

void AboutApp::Demolish()
{
    UNIMPLEMENTED(__func__);
}

void AboutApp::ToolClick(ClickableTool t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
}

} // namespace navitab
