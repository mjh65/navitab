/* This file is part of the Navitab project. See the README and LICENSE for details. */

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
    activeToolsMask(0),
    pendingToolsMask(0),
    repeatingToolsMask(0),
    pushedToolIdx(-1),
    longPressed(false)
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
    Update(ImageRegion(0, 0, width, height), nullptr);
}

void CoreToolbar::SetRepeatingTools(int selectMask)
{
    repeatingToolsMask = selectMask;
}

void CoreToolbar::RepaintTools(int statusTextWidth)
{
    // no need to repaint if there's been no change in the active tools
    // and the status text does not encroach on the tool icons
    bool overlap = (width - Window::TOOL_ICON_WIDTH * activeToolIds.size()) < statusTextWidth;
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
    unsigned numPendingTools = 0;
    unsigned numActiveTools = (int)activeToolIds.size();
    activeToolIds.clear();
    int x = width;
    for (int i = 0; i < kNumTools; ++i) {
        if (pendingToolsMask & (1 << i)) {
            ++numPendingTools;
            activeToolIds.push_back(i);
            x -= Window::TOOL_ICON_WIDTH;
            if (x >= 0) image->PaintIcon(x, 0, icons[i], Window::TOOL_ICON_WIDTH, Window::TOOLBAR_HEIGHT, backgroundPixels);
        }
    }
    while (numActiveTools > numPendingTools) {
        --numActiveTools;
        x -= Window::TOOL_ICON_WIDTH;
        if (x >= 0) image->PaintIcon(x, 0, tool_null_24x24, Window::TOOL_ICON_WIDTH, Window::TOOLBAR_HEIGHT, backgroundPixels);
    }
    activeToolsMask = pendingToolsMask;

    if (x < 0) x = 0;
    dirtyBits.push_back(ImageRegion(x, 0, width, height));
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
    // ignore mouse events that are not over the tool icons
    if ((y < 0) || (y >= Window::TOOLBAR_HEIGHT)) return;
    if ((x < (width - Window::TOOL_ICON_WIDTH * activeToolIds.size())) || (x >= width)) return;
    
    int toolIdx = (width - x) / Window::TOOL_ICON_WIDTH;
    assert(toolIdx < activeToolIds.size());
    int tool = activeToolIds[toolIdx];
    if (l) {
        std::chrono::time_point tn = std::chrono::steady_clock::now();
        if (pushedToolIdx < 0) {
            pushedToolIdx = toolIdx;
            longPressed = false;
            nextRepeatTime = tn + std::chrono::milliseconds(1000);
        } else if (toolIdx == pushedToolIdx) {
            if (tn > nextRepeatTime) {
                if (!longPressed) {
                    longPressed = true;
                    core->PostToolClick((ClickableTool)tool);
                } else {
                    if (repeatingToolsMask & (1 << tool)) {
                        core->PostToolClick((ClickableTool)tool);
                    }
                }
                nextRepeatTime = tn + std::chrono::milliseconds(500);
            }
        }
    } else {
        if ((toolIdx == pushedToolIdx) && !longPressed) {
            core->PostToolClick((ClickableTool)tool);
        }
        pushedToolIdx = -1;
        longPressed = false;
    }
}

void CoreToolbar::Update(navitab::ImageRegion r, uint32_t* pixels)
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
