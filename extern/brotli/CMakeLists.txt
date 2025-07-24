
set(BROTLI_DISABLE_TESTS ON)

add_subdirectory("${brotli_SOURCE_DIR}" "${brotli_BINARY_DIR}" EXCLUDE_FROM_ALL)

# This is to influence findPackage into detecting Navitab's version of BrotliDec
set(BROTLIDEC_INCLUDE_DIRS "${brotli_SOURCE_DIR}/c/include")
if(MSVC)
    set(BROTLIDEC_LIBRARIES "${brotli_BINARY_DIR}/brotlidec.lib" CACHE INTERNAL "")
else()
    set(BROTLIDEC_LIBRARIES "${brotli_BINARY_DIR}/libbrotlidec.a" CACHE INTERNAL "")
endif()
