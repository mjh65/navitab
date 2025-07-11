

FetchContent_Declare(gumbo
    URL "https://github.com/google/gumbo-parser/archive/refs/tags/v0.10.1.tar.gz"
    PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/patch.cmake"
)

FetchContent_MakeAvailable(gumbo)
