
set(BROTLI_DISABLE_TESTS ON)

add_subdirectory("${brotli_SOURCE_DIR}" "${brotli_BINARY_DIR}" EXCLUDE_FROM_ALL)

# This is to influence various uses of findPackage into detecting Navitab's version of BrotliDec
set(BROTLI_INCLUDE_DIR "${brotli_SOURCE_DIR}/c/include" CACHE INTERNAL "")
set(BROTLIDEC_INCLUDE_DIRS "${brotli_SOURCE_DIR}/c/include" CACHE INTERNAL "")
if(MSVC)
    set(BROTLICOMMON_LIBRARY "${brotli_BINARY_DIR}/brotlicommon.lib" CACHE INTERNAL "")
    set(BROTLIDEC_LIBRARY "${brotli_BINARY_DIR}/brotlidec.lib" CACHE INTERNAL "")
    set(BROTLIENC_LIBRARY "${brotli_BINARY_DIR}/brotlienc.lib")
else()
    set(BROTLICOMMON_LIBRARY "${brotli_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libbrotlicommon.a" CACHE INTERNAL "")
    set(BROTLIDEC_LIBRARY "${brotli_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libbrotlidec.a" CACHE INTERNAL "")
    set(BROTLIENC_LIBRARY "${brotli_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libbrotlienc.a")
endif()
set(BROTLIDEC_LIBRARIES ${BROTLICOMMON_LIBRARY} ${BROTLIDEC_LIBRARY} ${BROTLIENC_LIBRARY} CACHE INTERNAL "")

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_brotli_PKGLIBS brotlidec brotlienc brotlicommon CACHE INTERNAL "")
set(NAVITAB_brotlidec_LOCATION "${BROTLIDEC_LIBRARY}" CACHE INTERNAL "")
set(NAVITAB_brotlienc_LOCATION "${BROTLIENC_LIBRARY}" CACHE INTERNAL "")
set(NAVITAB_brotlicommon_LOCATION "${BROTLICOMMON_LIBRARY}" CACHE INTERNAL "")
