

FetchContent_Declare(lcms2
    URL "https://github.com/mm2/Little-CMS/releases/download/lcms2.17/lcms2-2.17.tar.gz"
    PATCH_COMMAND "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_LIST_DIR}/patch.cmake"
)

FetchContent_MakeAvailable(lcms2)

if(MSVC)
    set(LCMS2_LIBRARY "${lcms2_BINARY_DIR}/lcms2.lib" PARENT_SCOPE)
else()
    set(LCMS2_LIBRARY "${lcms2_BINARY_DIR}/liblcms2.a" PARENT_SCOPE)
endif()
set(LCMS2_INCLUDE_DIR "${lcms2_SOURCE_DIR}/include" PARENT_SCOPE)
set(LCMS2_INCLUDE_DIRS "${lcms2_SOURCE_DIR}/include" PARENT_SCOPE)
