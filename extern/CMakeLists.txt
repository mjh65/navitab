# This file is part of the Navitab project. See the README and LICENSE for details.

# This script handles all of the 3rd party libraries that are used by the
# Navitab project. A number of these projects do not support CMake natively,
# and in those cases a rudimentary CMake wrapper has been created and is
# patched into the source tree after downloading. The goal is to have a
# fully cross-platform build system (specifically supporting VS/MSVC).

set(NAVITAB_IMPORT_LIBS "${PROJECT_BINARY_DIR}/libs")
file(MAKE_DIRECTORY "${NAVITAB_IMPORT_LIBS}")
include(ImportExport)

# BUILD_NAVITAB_THIRDPARTY is ON by default. It builds the complete Navitab
# source tree, including the imported 3rd-party sources. If switched off then
# the CMake configuration will ignore the 3rd-party sources, assume the libraries
# are already built, and import them into the build configuration.

if(${BUILD_NAVITAB_THIRDPARTY})
    message(STATUS "Downloading and configuring third-party sources")

    # These packages are ordered so that dependencies can be fully satisfied within the
    # Navitab project so we can guarantee to keep all libraries static (Xplane requirement).
    set(PKGS
        fmt         "https://github.com/fmtlib/fmt/releases/download/10.2.1/fmt-10.2.1.zip"
        # fmt has no dependencies.
        json        "https://github.com/nlohmann/json/archive/refs/tags/v3.12.0.tar.gz"
        # json has no dependencies.
        sqlite      "https://www.sqlite.org/2025/sqlite-amalgamation-3500300.zip"
        # sqlite has no dependencies. it does not have inbuilt CMake support.
        lunasvg     "https://github.com/sammycage/lunasvg/archive/refs/tags/v3.3.0.zip"
        # lunasvg has no dependencies.
        zlib        "https://github.com/madler/zlib/releases/download/v1.3.1/zlib-1.3.1.tar.gz"
        # zlib has no dependencies.
        bzip2       "https://sourceware.org/pub/bzip2/bzip2-1.0.8.tar.gz"
        # bzip2 has no dependencies. it does not have inbuilt CMake support.
        brotli      "https://github.com/google/brotli/archive/refs/tags/v1.1.0.tar.gz"
        # brotli has no dependencies.
        jbig2       "https://github.com/ArtifexSoftware/jbig2dec/archive/refs/tags/0.20.tar.gz"
        # jbig2 appears to use zlib, and does not have inbuilt CMake support.
        lerc        "https://github.com/Esri/lerc/archive/refs/tags/v4.0.0.tar.gz"
        # lerc has no dependencies.
        zstd        "https://github.com/facebook/zstd/archive/refs/tags/v1.5.7.tar.gz"
        # ztsd has no dependencies.
        gumbo       "https://github.com/google/gumbo-parser/archive/refs/tags/v0.10.1.tar.gz"
        # gumbo has no dependencies. it does not have inbuilt CMake support.
        lcms2       "https://github.com/mm2/Little-CMS/releases/download/lcms2.17/lcms2-2.17.tar.gz"
        # lcms2 has no dependencies. it does not have inbuilt CMake support.
        jpeg        "https://www.ijg.org/files/jpegsrc.v9f.tar.gz"
        # jpeg has no dependencies. it does not have inbuilt CMake support.
        openjpeg    "https://github.com/uclouvain/openjpeg/archive/refs/tags/v2.5.3.tar.gz"
        # openjpeg has no dependencies.
        png         "https://github.com/pnggroup/libpng/archive/refs/tags/v1.6.50.tar.gz"
        # png requires zlib
        tiff        "https://gitlab.com/libtiff/libtiff/-/archive/v4.7.0/libtiff-v4.7.0.tar.gz"
        # tiff uses zlib, libjpeg, lerc, zstd
        # (tiff does not use deflate, jbig, liblzma, wbep, which are not currently provided or configured)
        mbedtls     "https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-3.6.4/mbedtls-3.6.4.tar.bz2"
        # mbedtls
        ssh2        "https://github.com/libssh2/libssh2/releases/download/libssh2-1.11.1/libssh2-1.11.1.tar.gz"
        # ssh2 is configured to use mbedtls
        curl        "https://curl.se/download/curl-8.15.0.tar.gz"
        # curl uses zlib, zstd, brotli, ssh2, mbedtls
        harfbuzz_NOCONF    "https://github.com/harfbuzz/harfbuzz/archive/refs/tags/11.2.1.tar.gz"
        # download and unpack harfbuzz, but do not configure yet, allows freetype access to headers
        freetype    "https://download.savannah.gnu.org/releases/freetype/freetype-2.13.3.tar.gz"
        # freetype uses zlib, bzip2, brotli, png, and harfbuzz (circular-dependency).
        harfbuzz    "https://github.com/harfbuzz/harfbuzz/archive/refs/tags/11.2.1.tar.gz"
        # harfbuzz uses freetype, but will use Navitab's freetype if it is a build target, so is processed afterwards
        mupdf       "https://github.com/ArtifexSoftware/mupdf/archive/refs/tags/1.26.3.tar.gz"
        # mupdf uses zlib, brotli, lcms2, jbig2, jpeg, openjpeg, freetype, harfbuzz, gumbo
        glfw        "https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.zip"
        # glfw has no dependencies.
        lvgl        "https://github.com/lvgl/lvgl/archive/refs/tags/v9.1.0.zip"
        # lvgl has no dependencies.

        # TODO - the following 3rd party libraries are used by Avitab, but have not
        # yet been brought into Navitab. It's possible they aren't even required.

        # url = https://github.com/hglm/detex
        # url = https://github.com/nothings/stb
        # url = https://github.com/OSGeo/proj.4
        # url = https://github.com/OSGeo/libgeotiff
        # url = https://github.com/ldarren/QuickJS/
        # url = https://github.com/gulrak/filesystem.git
    )

    # We don't want any of the packages to build shared libs, examples or testing.
    # These variables are common to more than one library, so we set them once here.
    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(BUILD_EXAMPLES OFF CACHE INTERNAL "")
    set(SKIP_INSTALL_ALL ON CACHE INTERNAL "")
    set(BUILD_TESTING_saved ${BUILD_TESTING})
    set(BUILD_TESTING OFF CACHE BOOL "Include tests in build" FORCE)

    # Download and configure each of the third-party packages in turn
    set(libs_thirdparty)
    include(Downloader)
    foreach(x ${PKGS})
        if("${pkg}" STREQUAL "")
            set(pkg ${x})
        else()
            IncludeDownloadedPackage(${pkg} "${x}" libs_thirdparty)
            unset(pkg)
        endif()
    endforeach()
    set(NAVITAB_ALL_PKGLIBS ${libs_thirdparty} CACHE INTERNAL "")

    set(navitab_3rdparty_copies)
    foreach(lib ${libs_thirdparty})
        set(libpath)
        ExportThirdPartyLibrary(${lib} libpath)
        set(navitab_3rdparty_copies ${navitab_3rdparty_copies} ${libpath})
    endforeach()
    add_dependencies(navitab_3rdparty ${navitab_3rdparty_copies})

    # Restore any cache variables that Navitab makes use of itself.
    set(BUILD_TESTING ${BUILD_TESTING_saved} CACHE BOOL "Include tests in build" FORCE)
    unset(BUILD_TESTING_saved)

    message(STATUS "Finished making external content available")
endif()

message(STATUS "Importing third-party libraries")
foreach(lib ${NAVITAB_ALL_PKGLIBS})
    ImportBuiltLibraries("${lib}" ${BUILD_NAVITAB_THIRDPARTY})
endforeach()
# declare the third party library dependencies after they have all been imported
foreach(lib ${NAVITAB_ALL_PKGLIBS})
    # required libraries
    foreach(tll ${NAVITAB_${lib}_LINKLIBS})
        target_link_libraries(${lib}_3rd INTERFACE ${tll}_3rd)
    endforeach()
endforeach()

if(WIN32)
    set(sys_libraries iphlpapi advapi32 crypt32 ws2_32 bcrypt)
elseif(APPLE)
    set(sys_libraries
        "-lm" "-framework SystemConfiguration" "-framework CoreFoundation"
        "-framework CoreServices" "-framework ApplicationServices"
        "-framework Cocoa" "-framework IOKit"
    )
endif()

target_link_libraries(navitab_3rdparty
    INTERFACE Threads::Threads
    fmt_3rd nlohmann_json_3rd curl_3rd mupdf_3rd lvgl_3rd sqlite3_3rd
    ${sys_libraries}
)
