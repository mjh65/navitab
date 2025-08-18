# This file is part of the Navitab project. See the README and LICENSE for details.

set(CRYPTO_BACKEND mbedTLS CACHE INTERNAL "")
set(LIBSSH2_NO_DEPRECATED ON CACHE INTERNAL "")

add_subdirectory("${ssh2_SOURCE_DIR}" "${ssh2_BINARY_DIR}" EXCLUDE_FROM_ALL)
add_library(ssh2 ALIAS libssh2_static)

unset(CRYPTO_BACKEND CACHE)
unset(LIBSSH2_NO_DEPRECATED CACHE)

# This is to influence findPackage into detecting Navitab's build of LibSSH2
set(LIBSSH2_INCLUDE_DIR "${ssh2_SOURCE_DIR}/include" CACHE INTERNAL "")
if(MSVC)
    set(LIBSSH2_LIBRARY "${NAVITAB_LIBS3RD_DIR}/ssh2.lib" CACHE INTERNAL "")
else()
    set(LIBSSH2_LIBRARY "${NAVITAB_LIBS3RD_DIR}/libssh2.a" CACHE INTERNAL "")
endif()

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_ssh2_PKGLIBS ssh2 CACHE INTERNAL "")
set(NAVITAB_ssh2_LOCATION "${LIBSSH2_LIBRARY}" CACHE INTERNAL "")
