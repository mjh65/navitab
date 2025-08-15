# This file is part of the Navitab project. See the README and LICENSE for details.

# Tool for rasterising SVG files to make icons for the UI.

target_sources(navitab_svgc PRIVATE
    main.cpp
)
target_compile_definitions(navitab_svgc PRIVATE
    ${NAVITAB_lunasvg_CPPDEFS}
)
target_include_directories(navitab_svgc PRIVATE
    ${NAVITAB_lunasvg_INCDIRS}
)
target_link_libraries(navitab_svgc PRIVATE
    lunasvg_3rd
)

set_target_properties(navitab_svgc PROPERTIES OUTPUT_NAME "svgc")
