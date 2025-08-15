# This file is part of the Navitab project. See the README and LICENSE for details.

# Desktop UI program using mock simulator, for interactive UI testing

add_executable(navitab_desktop WIN32)

target_sources(navitab_desktop PRIVATE main.cpp)

target_link_libraries(navitab_desktop PRIVATE
    navitab_core
    navitab_sim_mock
    navitab_win_glfw
)

if(WIN32)
    target_link_options(navitab_desktop PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/ENTRY:mainCRTStartup>
        $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
    )
    target_link_libraries(navitab_desktop PRIVATE
        opengl32
    )
elseif(APPLE)
    target_link_libraries(navitab_desktop PRIVATE
        "-framework OpenGL"
    )
elseif(UNIX)
    target_link_options(navitab_desktop PRIVATE
        -static -static-libgcc -static-libstdc++
    )
    target_link_libraries(navitab_desktop PRIVATE
        GL
    )
endif()

install(TARGETS navitab_desktop DESTINATION navitab/tools)
