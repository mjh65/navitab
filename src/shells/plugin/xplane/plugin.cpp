/*
 *  Navitab - Navigation Tablet for VR, derived from Folke Will's AviTab
 *  Copyright (c) 2024 Michael Hasling
 *  Copyright (c) 2018-2024 Folke Will
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

#include <memory>
#include <exception>
#include <cassert>
#include <fmt/core.h>
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlugin.h>
#include "navitab/core.h"
#include "navitab/xpplugin.h"
#include "navitab/logger.h"

// these are variables representing global state that will be referenced by all
// of the X-Plane plugin entry points.

std::unique_ptr<logging::Logger> LOG;
std::shared_ptr<navitab::CoreServices> nvt;
std::shared_ptr<navitab::XPlaneSimulator> sim;
// there is no separate management of the XPlane window here, it's handled by the sim envoy.

PLUGIN_API int XPluginStart(char* outName, char* outSignature, char* outDescription)
{
    strncpy(outName, "Navitab", 255);
    strncpy(outSignature, "com.github.navitab", 255);

    try {
        XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
        strncpy(outDescription, "A panel for maps, charts and documents when flying in VR", 255);
        // try to initialise logging and preferences - raises exception if fails
        LOG = std::make_unique<logging::Logger>("plugin");
        // construct the Navitab core, this will do enough to get the preferences
        nvt = navitab::CoreServices::MakeNavitab(navitab::SimEngine::XPLANE, navitab::WinServer::PLUGIN);
    }
    catch (const std::exception& e) {
        strncpy(outDescription, e.what(), 255);
        return 0;
    }

    assert(nvt);
    assert(LOG);
    try {
        LOGS("XPluginStart: early init completed");
        // start Navitab, and construct the XPlane simulation liaison
        nvt->Start();
        sim = navitab::XPlaneSimulator::Factory();
        sim->Connect(nvt);
        sim->Start();
        LOGS("XPluginStart: remaining init completed");
    }
    catch (const std::exception& e) {
        try {
            sim.reset();
            nvt.reset();
        }
        catch (...) {
            LOGE("Environment exception while destroying");
        }
        LOGE(fmt::format("Exception in XPluginStart: {}", e.what()));
        strncpy(outDescription, e.what(), 255);
        return 0;
    }
    assert(sim);
    return 1;
}

PLUGIN_API int XPluginEnable(void)
{
    assert(nvt);
    assert(LOG);
    try {
        // at enable, hook up the XPlane liaison with the Navitab core
        sim->Enable();
        nvt->Activate();
        LOGS("XPluginEnable: enable completed");
    }
    catch (const std::exception& e) {
        LOGE(fmt::format("Exception in XPluginEnable: {}", e.what()));
        return 0;
    }
    assert(sim);
    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID src, int msg, void* inParam)
{
    auto intParam = reinterpret_cast<intptr_t>(inParam);
    assert(sim);
    assert(nvt);
    assert(LOG);
    switch (msg) {
    case XPLM_MSG_PLANE_LOADED:
        // 0 is our plane. we don't care about the others!
        if (intParam == 0) sim->onPlaneLoaded();
        break;
    case XPLM_MSG_ENTERED_VR:
        sim->onVRmodeChange(true);
        break;
    case XPLM_MSG_EXITING_VR:
        sim->onVRmodeChange(false);
        break;
    }
}

PLUGIN_API void XPluginDisable(void)
{
    assert(sim);
    assert(nvt);
    assert(LOG);
    try {
        sim->Disable();
        nvt->Deactivate();
        LOGS("XPluginDisable: disable completed");
    }
    catch (const std::exception& e) {
        LOGE(fmt::format("Exception in XPluginDisable: {}", e.what()));
    }
}

PLUGIN_API void XPluginStop(void)
{
    assert(sim);
    assert(nvt);
    assert(LOG);
    LOGS("Navitab told to stop");
    try {
        sim->Stop();
        sim->Disconnect();
        sim.reset();
        nvt->Stop();
        nvt.reset();    // Navitab core will now shutdown gracefully
    }
    catch (const std::exception& e) {
        LOGE(fmt::format("Exception in XPluginStop: {}", e.what()));
    }

    LOGS("Navitab has now stopped");
}

#if defined(NAVITAB_WINDOWS)
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    return TRUE;
}
#endif
