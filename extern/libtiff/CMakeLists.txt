
# libtiff has cmake support which is quite old and generates warnings, and quite
# a lot of noise. for simplicity we've disabled support for various codecs that
# we can't be bothered to add speculatively. potentially there might be a need for
# these that will be discovered one day. at which point the problem can be resolved.

set(tiff-tools OFF CACHE INTERNAL "")
set(tiff-tests OFF CACHE INTERNAL "")
set(tiff-contrib OFF CACHE INTERNAL "")
set(tiff-docs OFF CACHE INTERNAL "")
set(libdeflate OFF CACHE INTERNAL "")
set(pixarlog OFF CACHE INTERNAL "")
set(jbig OFF CACHE INTERNAL "")
set(lzma OFF CACHE INTERNAL "")
set(webp OFF CACHE INTERNAL "")
set(ZSTD_HAVE_DECOMPRESS_STREAM ON)

add_subdirectory("${libtiff_SOURCE_DIR}" "${libtiff_BINARY_DIR}" EXCLUDE_FROM_ALL)

unset(tiff-tools CACHE)
unset(tiff-tests CACHE)
unset(tiff-contrib CACHE)
unset(tiff-docs CACHE)
unset(libdeflate CACHE)
unset(pixarlog CACHE)
unset(jbig CACHE)
unset(lzma CACHE)
unset(webp CACHE)

target_link_libraries(tiff
    INTERFACE libzstd
)

# this aims to help find_package select this tiff library to help out 
if(MSVC)
    set(TIFF_LIBRARY "${libtiff_BINARY_DIR}/libtiff/tiff.lib" CACHE INTERNAL "")
else()
    set(TIFF_LIBRARY "${libtiff_BINARY_DIR}/libtiff/libtiff.a" CACHE INTERNAL "")
endif()
set(TIFF_INCLUDE_DIR "${libtiff_SOURCE_DIR}/libtiff" CACHE INTERNAL "")
