# This file is part of the Navitab project. See the README and LICENSE for details.

# The Navitab plugin for the X-Plane simulator

add_library(xplane_plugin SHARED)

target_sources(xplane_plugin PRIVATE
    plugin.cpp
)

target_link_libraries(xplane_plugin PRIVATE
    navitab_core
    navitab_sim_xp
)

if(WIN32)
    target_compile_definitions(xplane_plugin PRIVATE IBM=1)
    target_link_options(xplane_plugin PRIVATE
        $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
    )
    target_link_libraries(xplane_plugin PRIVATE
        "${XPLANE_SDK}/Libraries/Win/XPLM_64.lib"
        opengl32
    )
    install(TARGETS xplane_plugin RUNTIME DESTINATION navitab/xplane/Navitab/win_x64)
elseif(APPLE)
    target_compile_definitions(xplane_plugin PRIVATE APL=1)
    target_link_options(xplane_plugin PRIVATE
        "-F${XPLANE_SDK}/Libraries/Mac"
    )
    target_link_libraries(xplane_plugin PRIVATE
        "-framework XPLM"
        "-framework OpenGL"
    )
    install(TARGETS xplane_plugin DESTINATION navitab/xplane/Navitab/mac_x64)
elseif(UNIX)
    target_compile_definitions(xplane_plugin PRIVATE LIN=1)
    target_link_options(xplane_plugin PRIVATE
        -static -static-libgcc -static-libstdc++
    )
    target_link_libraries(xplane_plugin PRIVATE
        GL
    )
    install(TARGETS xplane_plugin DESTINATION navitab/xplane/Navitab/lin_x64)
endif()

set_target_properties(xplane_plugin PROPERTIES PREFIX "")
set_target_properties(xplane_plugin PROPERTIES OUTPUT_NAME "Navitab")
set_target_properties(xplane_plugin PROPERTIES SUFFIX ".xpl")
