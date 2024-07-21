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
#include <XPLM/XPLMDefs.h>
#include <XPLM/XPLMPlugin.h>
#include "navitab/core.h"
#include "navitab/logger.h"
#include <fmt/core.h>

// these are variables representing global state that will be referenced by all
// of the X-Plane plugin entry points.

std::unique_ptr<navitab::System> nvt;
std::unique_ptr<navitab::logging::Logger> LOG;

PLUGIN_API int XPluginStart(char* outName, char* outSignature, char* outDescription)
{
    strncpy(outName, "Navitab", 255);
    strncpy(outSignature, "github.mjh65.navitab", 255);

    try {
        XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
        strncpy(outDescription, "A panel for maps, charts and documents when flying in VR", 255);
        // try to initialise logging and preferences - raises exception if fails
        nvt = navitab::System::GetSystem(navitab::Simulation::XPLANE, navitab::AppClass::PLUGIN);
        LOG = std::make_unique<navitab::logging::Logger>("plugin");
    }
    catch (const std::exception& e) {
        strncpy(outDescription, e.what(), 255);
        return 0;
    }

    try {
        LOGS("XPluginStart: early init completed");
        nvt->Start();
        LOGS("XPluginStart: later init completed");
    }
    catch (const std::exception& e) {
        try {
            //environment.reset();
        }
        catch (...) {
            LOGE("Environment exception while destroying");
        }
        LOGE(fmt::format("Exception in XPluginStart: {}", e.what()));
        strncpy(outDescription, e.what(), 255);
        return 0;
    }
    return 1;
}

PLUGIN_API int XPluginEnable(void)
{
    try {
        nvt->Enable();
        LOGS("XPluginEnable: enable completed");
    }
    catch (const std::exception& e) {
        LOGE(fmt::format("Exception in XPluginEnable: {}", e.what()));
        return 0;
    }
    return 1;
}

PLUGIN_API void XPluginReceiveMessage(XPLMPluginID src, int msg, void* inParam)
{
    if ((msg == XPLM_MSG_PLANE_LOADED) && (inParam == 0)) {
        // TODO if (nvt) nvt->onPlaneLoad();
    }
}

PLUGIN_API void XPluginDisable(void)
{
    try {
        nvt->Disable();
        LOGS("XPluginDisable: disable completed");
    }
    catch (const std::exception& e) {
        LOGE(fmt::format("Exception in XPluginDisable: {}", e.what()));
    }
}

PLUGIN_API void XPluginStop(void)
{
    LOGS("Navitab told to stop");
    try {
        nvt->Stop();
        nvt.reset();    // run destructors
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
