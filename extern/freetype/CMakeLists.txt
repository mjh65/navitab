

project(FreeType
    VERSION 2.13.3
    DESCRIPTION "A library for fonts"
    LANGUAGES C
)

execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/ftconfig.h" "${freetype_BINARY_DIR}/include/config/ftconfig.h")
execute_process(COMMAND "${CMAKE_COMMAND}" -E copy "${CMAKE_CURRENT_LIST_DIR}/ftoption.h" "${freetype_BINARY_DIR}/include/config/ftoption.h")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

set(FREETYPE_SOURCES pfr/pfr.c) # TODO!
PrefixSources(FREETYPE_SOURCES "${freetype_SOURCE_DIR}/src")

add_library(freetype STATIC ${FREETYPE_SOURCES})
target_compile_definitions(freetype
    PRIVATE FT2_BUILD_LIBRARY
)
target_include_directories(freetype
    PUBLIC "${freetype_BINARY_DIR}/include"
    PUBLIC "${freetype_SOURCE_DIR}/include"
    PUBLIC "${freetype_BINARY_DIR}/include/freetype/config"
)

