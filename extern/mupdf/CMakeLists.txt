

FetchContent_Declare(mupdf
    URL "https://github.com/ArtifexSoftware/mupdf/archive/refs/tags/1.26.3.tar.gz"
    PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/patch.cmake"
)

FetchContent_MakeAvailable(mupdf)

