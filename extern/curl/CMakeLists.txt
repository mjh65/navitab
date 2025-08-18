# This file is part of the Navitab project. See the README and LICENSE for details.

# these settings are more-or-less the equivalent of those used by Avitab.
set(BUILD_CURL_EXE OFF)
set(BUILD_STATIC_LIBS ON)
set(CURL_DISABLE_INSTALL ON)
set(CURL_STATIC_CRT ON)
set(CURL_USE_PKGCONFIG OFF)
set(CURL_ENABLE_EXPORT_TARGET OFF)
set(CURL_USE_MBEDTLS ON)
set(CURL_USE_OPENSSL OFF)
set(USE_NGHTTP2 OFF)
set(CURL_USE_LIBPSL OFF)
set(USE_LIBIDN2 OFF)
set(CURL_DISABLE_HTTP_AUTH ON)
set(CURL_DISABLE_FTP ON)
set(CURL_DISABLE_TFTP ON)
set(CURL_DISABLE_LDAP ON)
set(CURL_DISABLE_TELNET ON)
set(CURL_DISABLE_GOPHER ON)
set(CURL_DISABLE_DICT ON)
set(CURL_DISABLE_IMAP ON)
set(CURL_DISABLE_POP3 ON)
set(CURL_DISABLE_RTSP ON)
set(CURL_DISABLE_SMTP ON)
set(BUILD_LIBCURL_DOCS OFF)
set(BUILD_MISC_DOCS OFF)
set(ENABLE_CURL_MANUAL OFF)

add_subdirectory("${curl_SOURCE_DIR}" "${curl_BINARY_DIR}" EXCLUDE_FROM_ALL)
add_library(curl ALIAS libcurl_static)

target_link_libraries(libcurl_static
    INTERFACE zstd
    INTERFACE ssh2
    INTERFACE mbedtls
)

# Cache settings to be used when BUILD_NAVITAB_THIRDPARTY is OFF
set(NAVITAB_curl_PKGLIBS curl CACHE INTERNAL "")
if(MSVC)
    set(NAVITAB_curl_LOCATION "${NAVITAB_LIBS3RD_DIR}/curl.lib" CACHE INTERNAL "")
else()
    set(NAVITAB_curl_LOCATION "${NAVITAB_LIBS3RD_DIR}/libcurl.a" CACHE INTERNAL "")
endif()
set(NAVITAB_curl_INCDIRS "${curl_SOURCE_DIR}/include" CACHE INTERNAL "")
set(NAVITAB_curl_CPPDEFS CURL_STATICLIB CACHE INTERNAL "")
set(NAVITAB_curl_LINKLIBS zstd ssh2 mbedtls CACHE INTERNAL "")
