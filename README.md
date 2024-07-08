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
- try to support automated testing from the outset

## Configuring and Building

Run `cmake --preset release` to configure the workspace for building.

Other configure presets are `debug`, `xcode-debug`.

Run `cmake --build --preset all-release` to build all the Navitab products ready for
release.

Other build presets are `all-debug`, `desktop-xcode-debug`.

Navitab attempts to follow modern CMake recommendations, and should be buildable
on Windows, Linux, or MacOS systems with IDEs that support CMake, or using the
CMake command line directly.

Navitab is dependent on a number of 3rd party source code libraries which will be
downloaded and configured as part of the CMake configuration process. The very first
build may take some time, but subsequent builds should be quicker.

Although Navitab aims to be toolchain-agnostic, some of the 3rd-party libraries may
not be compatible with all toolchains. As a minimum Navitab can be built with gcc
(using the MinGW system for Windows).

Navitab is almost entirely written in C++. But some of the 3rd-party libraries
require additional toolchains and packages to be available in the environment.
A script `scripts/devenv_setup.sh` is provided to download the packages that are
required.
