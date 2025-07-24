

project(TEMPLATE
    VERSION 1.0.0
    DESCRIPTION "A library for something"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

set(TEMPLATE_SOURCES srcfile1.c)
PrefixSources(TEMPLATE_SOURCES "${template_SOURCE_DIR}")

add_library(template STATIC ${TEMPLATE_SOURCES})
target_compile_definitions(template
    PRIVATE FOO=1
)
target_include_directories(template
    PUBLIC "${template_SOURCE_DIR}"
)
