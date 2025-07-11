# Works with 3.23 and tested through 3.29
cmake_minimum_required(VERSION 3.23...3.29)

# Project name and a few useful settings. Other commands can pick up the results
project(BZIP2
    VERSION 1.0.8
    DESCRIPTION "A library for compression"
    LANGUAGES C
)

# Set things like CMAKE_CXX_STANDARD here
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Stuff needed to build the BZIP2 library.

# Use installed packages directly. Might need to add some fallback
# code later if any of these fail on other platforms.
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)

# The BZIP2 library
set(BZIP2_SOURCES blocksort.c bzlib.c compress.c crctable.c decompress.c huffman.c randtable.c)
set(BZIP2_HEADERS bzlib.h)

add_library(bzip2 STATIC ${BZIP2_SOURCES} ${BZIP2_HEADERS})
add_library(BZip2::bzip2 ALIAS bzip2)
target_include_directories(bzip2
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}"
)
