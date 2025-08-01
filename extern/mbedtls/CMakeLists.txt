

set(MBEDTLS_AS_SUBPROJECT ON CACHE INTERNAL "")
set(ENABLE_PROGRAMS OFF CACHE INTERNAL "")
set(ENABLE_TESTING OFF CACHE INTERNAL "")
set(MBEDTLS_FATAL_WARNINGS OFF CACHE INTERNAL "")
set(TEST_CPP OFF CACHE INTERNAL "")

add_subdirectory("${mbedtls_SOURCE_DIR}" "${mbedtls_BINARY_DIR}" EXCLUDE_FROM_ALL)

unset(MBEDTLS_AS_SUBPROJECT CACHE)
unset(ENABLE_PROGRAMS CACHE)
unset(ENABLE_TESTING CACHE)
unset(MBEDTLS_FATAL_WARNINGS CACHE)
unset(TEST_CPP CACHE)

# This is to influence findPackage into detecting Navitab's version of MbedTLS
set(MBEDTLS_INCLUDE_DIR "${mbedtls_SOURCE_DIR}/include" CACHE INTERNAL "")
if(MSVC)
    set(MBEDTLS_LIBRARY "${mbedtls_BINARY_DIR}/library/mbedtls.lib" CACHE INTERNAL "")
    set(MBEDX509_LIBRARY "${mbedtls_BINARY_DIR}/library/mbedx509.lib" CACHE INTERNAL "")
    set(MBEDCRYPTO_LIBRARY "${mbedtls_BINARY_DIR}/library/mbedcrypto.lib" CACHE INTERNAL "")
else()
    set(MBEDTLS_LIBRARY "${mbedtls_BINARY_DIR}/library/${XCODE_LIB_PATH_PREFIX}libmbedtls.a" CACHE INTERNAL "")
    set(MBEDX509_LIBRARY "${mbedtls_BINARY_DIR}/library/${XCODE_LIB_PATH_PREFIX}libmbedx509.a" CACHE INTERNAL "")
    set(MBEDCRYPTO_LIBRARY "${mbedtls_BINARY_DIR}/library/${XCODE_LIB_PATH_PREFIX}libmbedcrypto.a" CACHE INTERNAL "")
endif()
