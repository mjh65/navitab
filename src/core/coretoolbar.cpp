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
#include "coretoolbar.h"
#include "navitab.h"
#include "svg/tool_null_24x24.h"
#include "svg/tool_menu_24x24.h"
#include "svg/tool_cog_24x24.h"
#include "svg/tool_affirm_24x24.h"
#include "svg/tool_stop_24x24.h"
#include "svg/tool_cancel_24x24.h"
#include "svg/tool_reduce_24x24.h"
#include "svg/tool_centre_24x24.h"
#include "svg/tool_magnify_24x24.h"
#include "svg/tool_last_24x24.h"
#include "svg/tool_right_24x24.h"
#include "svg/tool_left_24x24.h"
#include "svg/tool_first_24x24.h"
#include "svg/tool_rotateright_24x24.h"
#include "svg/tool_rotateleft_24x24.h"
#include "svg/tool_bottom_24x24.h"
#include "svg/tool_down_24x24.h"
#include "svg/tool_up_24x24.h"
#include "svg/tool_top_24x24.h"

namespace navitab {

CoreToolbar::CoreToolbar(std::shared_ptr<Toolbar2Core> c, std::shared_ptr<lvglkit::Manager> u)
:   LOG(std::make_unique<logging::Logger>("toolbar")),
    core(c), uiMgr(u),
    lvhStatusInfo(0),
    numActiveTools(0),
    activeToolsMask(0),
    pendingToolsMask(0)
{
    uiDisplay = uiMgr->MakeDisplay(this);
}

CoreToolbar::~CoreToolbar()
{
}

void CoreToolbar::SetStausInfo(int zt, int fps, const Location& loc)
{
    auto now = navitab::LocalTime("%H:%M:%S");
    if (statusText.find(now) != 0) {
        // local time has changed, so recreate the toolbarStatus string
        statusText = now;
        statusText += fmt::format(" | {}fps", fps);
        int s = zt;
        int h = s / (60 * 60); s -= (h * 60 * 60);
        int m = s / 60; s -= (m * 60);
        statusText += fmt::format(" | {:02}:{:02}:{:02}Z", h, m, s);
        statusText += fmt::format(" | {:+.3f},{:+.3f}", loc.latitude, loc.longitude);
        LOGD(fmt::format("CoreToolbar::SetStausInfo({})", statusText));
        lv_label_set_text(lvhStatusInfo, statusText.c_str());
    }
}

void CoreToolbar::SetActiveTools(int selectMask)
{
    // called by the apps to indicate which toolbar tools should be shown
    pendingToolsMask = selectMask;
    Update(FrameRegion(0, 0, width, height), nullptr);
}

void CoreToolbar::RepaintTools(int statusTextWidth)
{
    // no need to repaint if there's been no change in the active tools
    // and the status text does not encroach on the tool icons
    bool overlap = (width - 24 * numActiveTools) < statusTextWidth;
    if ((activeToolsMask == pendingToolsMask) && !overlap) return;

    const uint32_t* icons[] = {
        tool_menu_24x24,
        tool_cog_24x24,
        tool_affirm_24x24,
        tool_stop_24x24,
        tool_cancel_24x24,
        tool_reduce_24x24,
        tool_centre_24x24,
        tool_magnify_24x24,
        tool_last_24x24,
        tool_right_24x24,
        tool_left_24x24,
        tool_first_24x24,
        tool_rotateright_24x24,
        tool_rotateleft_24x24,
        tool_bottom_24x24,
        tool_down_24x24,
        tool_up_24x24,
        tool_top_24x24
    };
    int numPendingTools = numActiveTools = 0;
    int x = width;
    for (int i = 0; i < kNumTools; ++i) {
        if (pendingToolsMask & (1 << i)) {
            ++numPendingTools;
            x -= 24;
            if (x >= 0) image->PaintIcon(x, 0, icons[i], 24, 24, backgroundPixels);
        }
        if (activeToolsMask & (1 << i)) {
            ++numActiveTools;
        }
    }
    while (numActiveTools > numPendingTools) {
        --numActiveTools;
        x -= 24;
        if (x >= 0) image->PaintIcon(x, 0, tool_null_24x24, 24, 24, backgroundPixels);
    }
    activeToolsMask = pendingToolsMask;
    numActiveTools = numPendingTools;
    if (x < 0) x = 0;

    dirtyBits.push_back(FrameRegion(x, 0, width, height));
}

void CoreToolbar::onResize(int w, int h)
{
    LOGD(fmt::format("CoreToolbar::onResize({},{})", w, h));
    width = w; height = Window::TOOLBAR_HEIGHT;

    // If the toolbar is resized then the previous image is just abandoned
    // and a new one is created. On the first resize notification the UI
    // widgets are created (using raw LVGL API - no wrappers!)
    image = std::make_unique<FrameBuffer>(width, height);
    uiDisplay->Resize(width, height, image->Row(0));
    activeToolsMask = 0; // force repaint of tools
    if (!lvhStatusInfo) {
        CreateWidgets();
    }
    uiDisplay->Select();
    lv_obj_invalidate(lv_screen_active());
}

void CoreToolbar::onMouseEvent(int x, int y, bool l, bool r)
{
    UNIMPLEMENTED(__func__ + fmt::format("({},{},{},{})", x, y, l, r));
}

void CoreToolbar::Update(navitab::FrameRegion r, uint32_t* pixels)
{
    // this is the update function called from the LVGL library
    LOGD(fmt::format("CoreToolbar::Update({},{}->{},{})", r.left, r.top, r.right, r.bottom));
    dirtyBits.push_back(r);
    RepaintTools(r.right);
    RunLater([this]() { Redraw(); });
}

void CoreToolbar::CreateWidgets()
{
    uiDisplay->Select();

    // Change the background colour
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(backgroundPixels), LV_PART_MAIN);

    // Create the status info text widget
    lvhStatusInfo = lv_label_create(lv_screen_active());
    lv_label_set_text(lvhStatusInfo, "Hello world");
    lv_obj_set_style_text_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(lvhStatusInfo, LV_ALIGN_LEFT_MID, 6, 0);
}

} // namespace navitab
