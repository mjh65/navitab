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

#include "readerapp.h"
#include "navitab/core.h"
#include "navitab/toolbar.h"

namespace navitab {

ReaderApp::ReaderApp(std::shared_ptr<AppServices> core)
:   App("rdrapp", core)
{
    defaultToolMask = 
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

void ReaderApp::Show()
{
    App::Show();
    core->EnableTools(defaultToolMask);
}

} // namespace navitab
