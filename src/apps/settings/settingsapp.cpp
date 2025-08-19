/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include <fmt/core.h>
#include "settingsapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

// TODO - the screen/dialog will be built dynamically by querying
// the settings manager (which knows which settings exist and can be modified by the UI)

SettingsApp::SettingsApp(std::shared_ptr<AppServices> core)
:   App("stgsapp", core)
{
    activeToolsMask = 
        (1 << ClickableTool::AFFIRM) |
        (1 << ClickableTool::CANCEL) |
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT) |
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP);
    repeatingToolsMask = 
        (1 << ClickableTool::RIGHT) |
        (1 << ClickableTool::LEFT) |
        (1 << ClickableTool::DOWN) |
        (1 << ClickableTool::UP);
}

void SettingsApp::Assemble()
{
    lv_obj_t * label = lv_label_create(root);
    lv_label_set_text(label, "Navitab Settings\nTBD");
    lv_obj_center(label);
}

void SettingsApp::Demolish()
{
    UNIMPLEMENTED(__func__);
}

void SettingsApp::ToolClick(ClickableTool t)
{
    UNIMPLEMENTED(__func__ + fmt::format("({})", (int)t));
}

} // namespace navitab
