# This file is part of the Navitab project. See the README and LICENSE for details.

# GLFW project - provides OpenGL window manager, etc

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

add_subdirectory("${glfw_SOURCE_DIR}" "${glfw_BINARY_DIR}" EXCLUDE_FROM_ALL)
add_library(glfw3 ALIAS glfw)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_glfw_PKGLIBS glfw3 CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_glfw3_LOCATION "${NAVITAB_IMPORT_LIBS}/glfw3.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_glfw3_LOCATION "${NAVITAB_IMPORT_LIBS}/libglfw3.a" CACHE INTERNAL "")
endif()
set(NAVITAB_glfw3_INCDIRS "${glfw_SOURCE_DIR}/include" CACHE INTERNAL "")
