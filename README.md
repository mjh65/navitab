# Navitab

A speculative alternative to AviTab, drawing heavily on the original's
source code and ideas.

## Why?

Why? That's harder to justify. Part of the motivation was to be able to do
some more risky and disruptive changes that might not be justified in an
actively-maintained product. For example:

- changing the build system to follow more modern CMake practices
- upgrade some of the external libraries to the latest versions
- play around with the UI design to improve usable screen space
- change some internal structuring to assist with MSFS porting
- add some automated testing
- add some generated documentation

Another motivation was to do something non-trivial to keep the tech-hemisphere
of my brain from atrophy. This is a great justification for starting an
unfeasible project, solving a few interesting problems, and then giving up in
favour of some other bright, shiny, bauble of a different project. So let's see
where this goes ...

## Configuring and Building

Navitab attempts to follow modern CMake recommendations, and should be buildable
on Windows, Linux, or MacOS systems with IDEs that support CMake, or using the
CMake command line directly.

### IDE

If using an IDE that supports CMake (eg Visual Studio, VS Code, ...) then it should
be possible to open the top-level folder in the IDE. On first use one of the
configuration presets may be requested - either `debug` or `release` are
appropriate.

If using an IDE that does not support CMake (eg Xcode) then it will first be
necessary to use cmake to generate a native project for the IDE in question.
Navitab currently provides a configuration preset `xcode` for Apple's XCode IDE.

### Shell/command-line

Run `cmake --list-presets` to show all the current presets (in case this README
is not up to date).

Run `cmake --preset release` to configure the project for a stripped and optimised
build, or `cmake --preset debug` for development and debugging. Both of these builds
will put the Navitab products into the install folder (typically `install` in the
top-level project directory.)

(There is also a CMake configuration `xcode` to generate an Xcode project, see above).

Run `cmake --build --preset release` (or `cmake --build --preset debug`) to build
the Navitab products and create the installation files. These builds use the CMake
configuration of the same name. The installation files will be placed in the directory
`${CMAKE_INSTALL_PREFIX}`, which defaults to `install` in the project's top-level.

### Generated build optimisation

An optimised build will be generated if the CMake scripts detect that the 3rd-party
libraries have already been built. Under most circumstances this is desirable
since the sources of the 3rd-party libraries are not expected to be changed during
Navitab development.

The recommended recipe for Navitab development from an empty build directory is:

1. Use cmake to configure the build. The 3rd-party library sources will be included in the generated build.
2. Build the Navitab project. This will take some time.
3. Use cmake to reconfigure the build. Since the 3rd-party library binaries are available the sources will not be included in the generated build.
4. Develop and build the Navitab project as normal.

To override the automatic optimisation behaviour and force the CMake scripts to always include the 3rd-party
library sources set the CMake cache variable `BUILD_NAVITAB_THIRDPARTY` to `ON`.

The cache variable can be changed by editing the CMakeCache.txt file, and this is
the usual approach when using an IDE that supports CMake.

Alternatively, if using the cmake commands from a shell, the cache variable can be set
on the command line. Run `cmake --preset release -DBUILD_NAVITAB_THIRDPARTY=ON`
(or the debug equivalent) to reconfigure the build.

### Platforms

Navitab should be buildable on Windows, Linux and Mac. The GNU gcc toolchain
should be supported on all platforms. (On Windows gcc is supported under MinGW).
In addition Apple's clang and Microsoft's Visual C++ should be supported as
appropriate. Other toolchains may work, but are unlikely to be tested.

### Dependencies

Navitab is dependent on a number of 3rd-party source code libraries which are
downloaded and configured as part of the CMake configuration process. The first
configuration and build will take some time, but subsequent iterations builds
should be quicker. In particular a custom source package cache is created in the
top-level build directory which is shared between the build configurations (Debug,
Release, etc). There should be no reason to delete these packages after the first
download, and this will make future re-configurations slightly faster.

Furthermore, after the first complete build, running cmake will generate an optimised
build that treats the 3rd-party libraries as installed binary packages, reducing
the size of the build graph. See the earlier section on build optimisation for details.

Navitab is almost entirely written in C++. Some of the 3rd-party libraries
may require additional toolchains and packages to be available in the environment.
(This will become apparent when the build fails!)

As a minimum Navitab should be buildable with GNU gcc, Microsoft VC++, and
Apple clang.

### Environment variables

Some of the Navitab features and build configuration are dependent on environment
variables.

`MSFS_SDK`: if set should be the location of the (latest) SDK for Microsoft
Flight Simulator. If not set then the MSFS integration package will not be built.

`XPLANE_SDK`: if set should be the location of the (latest) SDK for X-Plane.
If not set then the X-Plane plugin package will not be built.

`CHARTFOX_CLIENTID`: this is required to build Chartfox integration into
the Navitab core libraries. Navitab users will still be required to authenticate
with Chartfox before the integration will become active.

## Licensing and copyright

Navitab is licensed under the GNU Affero General Public License, see the
file LICENSE for the full text.

The concept and various portions of the source code are derived from Avitab.
Those portions of source code are Copyright (C) Folke Will <folko@solhost.org>.

Navitab uses the following 3rd-party libraries and acknowledges the Copyright
and Licensing of each, details can be found in the downloaded packages, the
sources of which are listed in the file `extern/CMakeLists.txt`:

brotli, bzip2, curl, fmt, freetype, glfw, gumbo-parser, harfbuzz, jbig2dec,
libjpeg, nlohmann-json, lcms2, lerc, lunasvg, lvgl, mbedtls, mupdf, openjpeg,
libpng, sqlite3, libssh2, libtiff, zlib, zstd.
