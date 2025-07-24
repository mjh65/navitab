
# Project name and a few useful settings. Other commands can pick up the results
project(LCMS2
    VERSION 2.2.17
    DESCRIPTION "A library for color management"
    LANGUAGES C
)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
find_package(Threads REQUIRED)
include(Prefixer)

# The LCMS2 library
set(LCMS2_SOURCES
    cmsalpha.c
    cmscam02.c
    cmscgats.c
    cmscnvrt.c
    cmserr.c
    cmsgamma.c
    cmsgmt.c
    cmshalf.c
    cmsintrp.c
    cmsio0.c
    cmsio1.c
    cmslut.c
    cmsmd5.c
    cmsmtrx.c
    cmsnamed.c
    cmsopt.c
    cmspack.c
    cmspcs.c
    cmsplugin.c
    cmsps2.c
    cmssamp.c
    cmssm.c
    cmstypes.c
    cmsvirt.c
    cmswtpnt.c
    cmsxform.c
)
PrefixSources(LCMS2_SOURCES "${lcms2_SOURCE_DIR}/src")

add_library(lcms2 STATIC ${LCMS2_SOURCES})
target_include_directories(lcms2
    PUBLIC "${lcms2_SOURCE_DIR}/include"
)
