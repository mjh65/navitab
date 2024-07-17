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
#include "navitab/logging.h"
#include <fmt/core.h>

// these are variables representing global state that will be referenced by all
// of the X-Plane plugin entry points.

std::unique_ptr<navitab::core::Navitab> nvt;
std::unique_ptr<navitab::logging::Logger> logger;

PLUGIN_API int XPluginStart(char* outName, char* outSignature, char* outDescription)
{
    strncpy(outName, "Navitab", 255);
    strncpy(outSignature, "github.mjh65.navitab", 255);

    try {
        XPLMEnableFeature("XPLM_USE_NATIVE_PATHS", 1);
        strncpy(outDescription, "A panel for maps, charts and documents when flying in VR", 255);
        // try to initialise logging and preferences - raises exception if fails
        nvt = std::make_unique<navitab::core::Navitab>(navitab::core::Simulation::XPLANE, navitab::core::AppClass::PLUGIN);
        logger = std::make_unique<navitab::logging::Logger>("plugin");
    }
    catch (const std::exception& e) {
        strncpy(outDescription, e.what(), 255);
        return 0;
    }

    auto LOG = (*logger);
    try {
        zSTATUS(LOG,"XPluginStart: early init completed");
        nvt->start();
        zSTATUS(LOG,"XPluginStart: later init completed");
    }
    catch (const std::exception& e) {
        try {
            //environment.reset();
        }
        catch (...) {
            zERROR(LOG,"Environment exception while destroying");
        }
        zERROR(LOG,fmt::format("Exception in XPluginStart: {}", e.what()));
        strncpy(outDescription, e.what(), 255);
        return 0;
    }
    return 1;
}

PLUGIN_API int XPluginEnable(void)
{
    auto LOG = (*logger);
    try {
        nvt->enable();
        zSTATUS(LOG,"XPluginEnable: enable completed");
    }
    catch (const std::exception& e) {
        zERROR(LOG, fmt::format("Exception in XPluginEnable: {}", e.what()));
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
    auto LOG = (*logger);
    try {
        nvt->disable();
        zSTATUS(LOG,"XPluginDisable: disable completed");
    }
    catch (const std::exception& e) {
        zERROR(LOG, fmt::format("Exception in XPluginDisable: {}", e.what()));
    }
}

PLUGIN_API void XPluginStop(void)
{
    auto LOG = (*logger);
    zSTATUS(LOG,"Navitab told to stop");
    try {
        nvt->stop();
        nvt.reset();    // run destructors
    }
    catch (const std::exception& e) {
        zERROR(LOG, fmt::format("Exception in XPluginStop: {}", e.what()));
    }

    zSTATUS(LOG,"Navitab has now stopped");
}

#if defined(NAVITAB_WINDOWS)
#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    return TRUE;
}
#endif
