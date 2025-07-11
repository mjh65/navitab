

set(BUILD_EXECUTABLES OFF CACHE INTERNAL "")

FetchContent_Declare(libjpeg
    URL "https://github.com/csparker247/jpeg-cmake/archive/refs/tags/v1.3.0.tar.gz"
)

FetchContent_MakeAvailable(libjpeg)

target_include_directories(jpeg_static
    INTERFACE "$<BUILD_INTERFACE:${libjpeg_SOURCE_DIR}/libjpeg>"
)
