# This file is part of the Navitab project. See the README and LICENSE for details.

project(SQLITE3
    VERSION 1.0.0
    DESCRIPTION "A relational database library"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)

add_library(sqlite3 STATIC "${sqlite_SOURCE_DIR}/sqlite3.c")

target_compile_definitions(sqlite3 PRIVATE
    SQLITE_DQS=0
    SQLITE_DEFAULT_MEMSTATUS=0
    SQLITE_DEFAULT_SYNCHRONOUS=0
    SQLITE_DEFAULT_WAL_SYNCHRONOUS=0
    SQLITE_MAX_EXPR_DEPTH=0
    SQLITE_OMIT_DECLTYPE
    SQLITE_OMIT_DEPRECATED
    SQLITE_OMIT_LOAD_EXTENSION
    SQLITE_OMIT_PROGRESS_CALLBACK
    SQLITE_OMIT_SHARED_CACHE
    SQLITE_USE_ALLOCA
    SQLITE_OMIT_AUTOINIT
)

target_include_directories(sqlite3
    PUBLIC "${sqlite_SOURCE_DIR}"
)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_sqlite_PKGLIBS sqlite3 CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_sqlite3_LOCATION "${NAVITAB_LIBS3RD_DIR}/sqlite3.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_sqlite3_LOCATION "${NAVITAB_LIBS3RD_DIR}/libsqlite3.a" CACHE INTERNAL "")
endif()
set(NAVITAB_sqlite3_INCDIRS "${sqlite_SOURCE_DIR}" CACHE INTERNAL "")
