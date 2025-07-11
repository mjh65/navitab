

FetchContent_Declare(BZip2
    URL "https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz"
    PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/patch.cmake"
    OVERRIDE_FIND_PACKAGE
)

FetchContent_MakeAvailable(BZip2)

set(BZIP2_INCLUDE_DIRS "${bzip2_SOURCE_DIR}" PARENT_SCOPE)
