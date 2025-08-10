# This file is part of the Navitab project. See the README and LICENSE for details.

set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h)
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON)
set(LV_CONF_BUILD_DISABLE_DEMOS ON)

add_subdirectory("${lvgl_SOURCE_DIR}" "${lvgl_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_lvgl_PKGLIBS lvgl CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_lvgl_LOCATION "${lvgl_BINARY_DIR}/lvgl.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_lvgl_LOCATION "${lvgl_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}liblvgl.a" CACHE INTERNAL "")
endif()
set(NAVITAB_lvgl_INCDIRS "${PROJECT_SOURCE_DIR}/extern/lvgl" "${lvgl_SOURCE_DIR}" CACHE INTERNAL "")
set(NAVITAB_lvgl_CPPDEFS LV_LVGL_H_INCLUDE_SIMPLE LV_CONF_INCLUDE_SIMPLE "LV_CONF_PATH=${PROJECT_SOURCE_DIR}/extern/lvgl/lv_conf.h" CACHE INTERNAL "")
