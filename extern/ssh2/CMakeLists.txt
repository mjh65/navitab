

set(CRYPTO_BACKEND mbedTLS CACHE INTERNAL "")
set(LIBSSH2_NO_DEPRECATED ON CACHE INTERNAL "")

add_subdirectory("${ssh2_SOURCE_DIR}" "${ssh2_BINARY_DIR}" EXCLUDE_FROM_ALL)

unset(CRYPTO_BACKEND CACHE)
unset(LIBSSH2_NO_DEPRECATED CACHE)

# This is to influence findPackage into detecting Navitab's build of LibSSH2
set(LIBSSH2_INCLUDE_DIR "${ssh2_SOURCE_DIR}/include" CACHE INTERNAL "")
if(MSVC)
    set(LIBSSH2_LIBRARY "${ssh2_BINARY_DIR}/src/libssh2.lib" CACHE INTERNAL "")
else()
    set(LIBSSH2_LIBRARY "${ssh2_BINARY_DIR}/src/${XCODE_LIB_PATH_PREFIX}libssh2.a" CACHE INTERNAL "")
endif()
