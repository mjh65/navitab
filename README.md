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
necessary to use cmake to generate a build project for the IDE in question.
Navitab currently provides a configuration preset `xcode` for Apple's XCode IDE.

### Shell/command-line

Run `cmake --preset release` to configure the project for a stripped and optimised
build, or `cmake --preset debug` for development and debugging.

(There is also a CMake configuration `xcode` to generate an Xcode project, see above).

Run `cmake --build --preset release` (or `cmake --build --preset debug`) to build
all the Navitab products.

### Platforms

Navitab should be buildable on Windows, Linux and Mac. The GNU gcc toolchain
should be supported on all platforms. (On Windows gcc is supported under MinGW).
In addition Apple's clang and Microsoft's Visual C++ should be supported as
appropriate. Other toolchains may work, but are unlikely to be tested.

### Dependencies

Navitab is dependent on a number of 3rd party source code libraries which will be
downloaded and configured as part of the CMake configuration process. The first
configuration and build may take some time, but subsequent builds should be quicker.

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

