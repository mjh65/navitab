


add_subdirectory("${lerc_SOURCE_DIR}" "${lerc_BINARY_DIR}" EXCLUDE_FROM_ALL)


# This is to influence findPackage into detecting Navitab's version of Lerc
set(LERC_INCLUDE_DIR "${lerc_SOURCE_DIR}/src/LercLib/include" CACHE INTERNAL "")
if(MSVC)
    set(LERC_LIBRARY "${lerc_BINARY_DIR}/lerc.lib" CACHE INTERNAL "")
else()
    set(LERC_LIBRARY "${lerc_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}liblerc.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_lerc_PKGLIBS lerc CACHE INTERNAL "")
set(NAVITAB_lerc_LOCATION "${LERC_LIBRARY}" CACHE INTERNAL "")
