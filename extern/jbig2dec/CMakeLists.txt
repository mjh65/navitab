

FetchContent_Declare(JBIG2
    URL "https://github.com/ArtifexSoftware/jbig2dec/archive/refs/tags/0.20.tar.gz"
    PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/patch.cmake"
)

FetchContent_MakeAvailable(JBIG2)
