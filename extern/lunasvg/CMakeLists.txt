# This file is part of the Navitab project. See the README and LICENSE for details.

# LunaSVG provides libraries for rendering SVG images to bitmaps.

add_subdirectory("${lunasvg_SOURCE_DIR}" "${lunasvg_BINARY_DIR}" EXCLUDE_FROM_ALL)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_lunasvg_PKGLIBS lunasvg plutovg CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_lunasvg_LOCATION "${NAVITAB_IMPORT_LIBS}/lunasvg.lib" CACHE INTERNAL "")
    set(NAVITAB_plutovg_LOCATION "${NAVITAB_IMPORT_LIBS}/plutovg.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_lunasvg_LOCATION "${NAVITAB_IMPORT_LIBS}/liblunasvg.a" CACHE INTERNAL "")
    set(NAVITAB_plutovg_LOCATION "${NAVITAB_IMPORT_LIBS}/libplutovg.a" CACHE INTERNAL "")
endif()
set(NAVITAB_lunasvg_CPPDEFS "LUNASVG_BUILD_STATIC" CACHE INTERNAL "")
set(NAVITAB_lunasvg_INCDIRS "${lunasvg_SOURCE_DIR}/include" CACHE INTERNAL "")
set(NAVITAB_lunasvg_LINKLIBS plutovg CACHE INTERNAL "")
