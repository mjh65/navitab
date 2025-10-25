# This file is part of the Navitab project. See the README and LICENSE for details.

# Windows app using mock simulator, for interactive UI testing

add_executable(navitab_msfs_app WIN32)

target_sources(navitab_msfs_app PRIVATE main.cpp)

target_link_libraries(navitab_msfs_app PRIVATE
    navitab_core
    navitab_sim_msfs
    navitab_win_glfw
)

target_link_options(navitab_msfs_app PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/ENTRY:mainCRTStartup>
    $<$<CXX_COMPILER_ID:GNU>:-static -static-libgcc -static-libstdc++>
)
target_link_libraries(navitab_msfs_app PRIVATE
    opengl32
)

add_custom_command(TARGET navitab_msfs_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE:msfs_simconnect_dll> $<TARGET_FILE_DIR:navitab_msfs_app>
)

install(TARGETS navitab_msfs_app DESTINATION navitab/msfs/navitab-desktop-app)
install(FILES $<TARGET_FILE:msfs_simconnect_dll> DESTINATION navitab/msfs/navitab-desktop-app)
