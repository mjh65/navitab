# This file is part of the Navitab project. See the README and LICENSE for details.

# Tool for rasterising SVG files to make icons for the UI.

target_sources(navitab_svgc PRIVATE
    main.cpp
)

# this is required to workaround a bug in lunasvg v2.4.1
target_compile_definitions(navitab_svgc PRIVATE
    LUNASVG_BUILD_STATIC
)

target_link_libraries(navitab_svgc PRIVATE
    lunasvg
)

set_target_properties(navitab_svgc PROPERTIES OUTPUT_NAME "svgc")
