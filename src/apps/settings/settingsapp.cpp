/*
 *  Navitab - Navigation tablet for VR flight simulation
 *  Copyright (C) 2024 Michael Hasling
 *  Significantly derived from Avitab
 *  Copyright (C) 2018-2024 Folke Will <folko@solhost.org>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <fmt/core.h>
#include "settingsapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

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
