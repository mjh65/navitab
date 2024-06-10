#!/bin/bash

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
