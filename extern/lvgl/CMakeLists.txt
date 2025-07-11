
set(LV_CONF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/lv_conf.h CACHE INTERNAL "")
set(LV_CONF_BUILD_DISABLE_EXAMPLES ON CACHE INTERNAL "")
set(LV_CONF_BUILD_DISABLE_DEMOS ON CACHE INTERNAL "")

FetchContent_Declare(lvgl
    URL "https://github.com/lvgl/lvgl/archive/refs/tags/v9.1.0.zip"
    PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/patch.cmake"
)

FetchContent_MakeAvailable(lvgl)
