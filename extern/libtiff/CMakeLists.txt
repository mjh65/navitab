

SET(tiff-tools OFF CACHE INTERNAL "")
SET(tiff-tests OFF CACHE INTERNAL "")
SET(tiff-contrib OFF CACHE INTERNAL "")
SET(tiff-docs OFF CACHE INTERNAL "")

FetchContent_Declare(libtiff
    URL "https://gitlab.com/libtiff/libtiff/-/archive/v4.7.0/libtiff-v4.7.0.tar.gz"
)

FetchContent_MakeAvailable(libtiff)

if(MSVC)
    set(TIFF_LIBRARY "${libtiff_BINARY_DIR}/libtiff/tiff.lib" PARENT_SCOPE)
else()
    set(TIFF_LIBRARY "${libtiff_BINARY_DIR}/libtiff/libtiff.a" PARENT_SCOPE)
endif()
set(TIFF_INCLUDE_DIR "${libtiff_SOURCE_DIR}/libtiff" PARENT_SCOPE)
