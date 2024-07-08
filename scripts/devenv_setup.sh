#!/bin/bash

#   Navitab - Navigation Tablet for VR, derived from Folke Will's AviTab
#   Copyright (c) 2024 Michael Hasling
#   Copyright (c) 2018-2024 Folke Will
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <https://www.gnu.org/licenses/>.

# This script will check that the development environment contains all of the
# tools required to build Navitab, and will install any that are missing.

aptInstall() {
  sudo apt install -y "$1"
}
linuxSetup() {
  aptInstall cmake
  aptInstall make
  aptInstall autoconf
  aptInstall automake
  aptInstall libtool
  aptInstall libglfw3
  aptInstall libglfw3-dev
  aptInstall uuid-dev
}

pacmanInstall() {
  pacman -S "$1"
}
msys2Setup() {
  if pacman --version 2>&1 >/dev/null; then
    pacmanInstall mingw-w64-x86_64-toolchain
    pacmanInstall mingw64/mingw-w64-x86_64-cmake
    pacmanInstall msys/git
    pacmanInstall msys/patch
    pacmanInstall msys/make
    pacmanInstall msys/autoconf
    pacmanInstall msys/automake
    pacmanInstall msys/libtool
    pacmanInstall mingw64/mingw-w64-x86_64-glfw
  else
    echo "Windows development requires MSYS2, but pacman was not found, so something is not right."
    echo "Check the MSYS2 installation, if not installed it can be downloaded from https://www.msys2.org/"
    echo "After installation start an MSYS2 terminal and try running this script again."
    exit 1
  fi
}

brewInstall() {
  brew install "$1"
}
macosSetup() {
  if brew --version 2>&1 >/dev/null; then
    brewInstall cmake
    brewInstall make
    brewInstall autoconf
    brewInstall automake
    brewInstall libtool
    brewInstall glfw
    brewInstall pkgconfig
    brewInstall rust
  else
    echo "MacOS development setup requires brew, but it was not found, so something is not right."
    echo "Please download it from: https://brew.sh/ and install it, then run this script again."
    exit
  fi
}

# OS Detection
case "$OSTYPE" in
linux*)
  echo "Linux detected..."
  linuxSetup
  ;;
msys*)
  echo "Windows detected..."
  msys2Setup
  ;;
darwin*)
  echo "macOS detected..."
  macosSetup
  ;;
*)
  echo "Unkown system, exiting..."
  exit
  ;;
esac
