
FetchContent_Declare(openjpg
    URL "https://github.com/uclouvain/openjpeg/archive/refs/tags/v2.5.3.tar.gz"
)

FetchContent_MakeAvailable(openjpg)

unset(EXECUTABLE_OUTPUT_PATH CACHE)
unset(LIBRARY_OUTPUT_PATH CACHE)

target_include_directories(openjp2
    INTERFACE "$<BUILD_INTERFACE:${openjpg_SOURCE_DIR}/src/lib/openjp2>"
    INTERFACE "$<BUILD_INTERFACE:${openjpg_BINARY_DIR}/src/lib/openjp2>"
)
