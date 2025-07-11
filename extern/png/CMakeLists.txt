
set(PNG_SHARED OFF CACHE INTERNAL "")
set(PNG_TOOLS OFF CACHE INTERNAL "")
set(PNG_TESTS OFF CACHE INTERNAL "")
set(SKIP_INSTALL_EXPORT ON CACHE INTERNAL "")
set(SKIP_INSTALL_CONFIG_FILE ON CACHE INTERNAL "")

FetchContent_Declare(png
    URL "https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.50.tar.gz"
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(PNG)

add_library(PNG::PNG ALIAS png_static)

set(PNG_INCLUDE_DIRS "${png_SOURCE_DIR}" "${png_BINARY_DIR}" PARENT_SCOPE)
