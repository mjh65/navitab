
set(PNG_STATIC ON)
set(PNG_SHARED OFF)
if(APPLE)
    set(PNG_FRAMEWORK OFF)
endif()
set(PNG_TOOLS OFF)
set(PNG_TESTS OFF)
set(SKIP_INSTALL_EXPORT ON)
set(SKIP_INSTALL_CONFIG_FILE ON)

add_subdirectory("${png_SOURCE_DIR}" "${png_BINARY_DIR}" EXCLUDE_FROM_ALL)

target_include_directories(png_static
    INTERFACE "$<BUILD_INTERFACE:${png_SOURCE_DIR}>"
    INTERFACE "$<BUILD_INTERFACE:${png_BINARY_DIR}>"
)
