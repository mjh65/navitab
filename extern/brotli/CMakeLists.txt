
set(BROTLI_DISABLE_TESTS ON)

add_subdirectory("${brotli_SOURCE_DIR}" "${brotli_BINARY_DIR}" EXCLUDE_FROM_ALL)

# This is to influence findPackage into detecting Navitab's version of BrotliDec
set(BROTLI_INCLUDE_DIR "${brotli_SOURCE_DIR}/c/include" CACHE INTERNAL "")
if(MSVC)
    set(BROTLICOMMON_LIBRARY "${brotli_BINARY_DIR}/brotlicommon.lib" CACHE INTERNAL "")
    set(BROTLIDEC_LIBRARY "${brotli_BINARY_DIR}/brotlidec.lib" CACHE INTERNAL "")
else()
    set(BROTLICOMMON_LIBRARY "${brotli_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libbrotlicommon.a" CACHE INTERNAL "")
    set(BROTLIDEC_LIBRARY "${brotli_BINARY_DIR}/${XCODE_LIB_PATH_PREFIX}libbrotlidec.a" CACHE INTERNAL "")
endif()
