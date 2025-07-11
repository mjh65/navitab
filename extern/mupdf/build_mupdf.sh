#!/usr/bin/env bash

# Build the MuPDF libraries

# Args scriptdir, srcdir, outlib

make -C "$2" XCFLAGS=-msse4.1 -j8
