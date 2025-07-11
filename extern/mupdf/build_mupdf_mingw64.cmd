@ECHO ON
:: Build the MuPDF libraries under the MinGW64 system.

:: Args - srcdir, outdir 
IF [%1] EQU [] EXIT 1
IF [%2] EQU [] EXIT 1

:: Normalise the directory paths. CD is tolerant to / seperator, but not all commands are
CD "%2"
set "_outd=%CD%"
CD "%1"
set "_srcd=%CD%"

IF EXIST "%_outd%\libmupdf.a" DEL "%_outd%\libmupdf.a"
IF EXIST "%_outd%\libmupdf-third.a" DEL "%_outd%\libmupdf-third.a"

C:\msys64\usr\bin\env MSYSTEM=MINGW64 CHERE_INVOKING=1 /usr/bin/bash -lc "make XCFLAGS=-msse4.1 -j8"

IF %ERRORLEVEL% NEQ 0 EXIT 1
IF NOT EXIST "%_srcd%\build\release\libmupdf.a" EXIT 1
IF NOT EXIST "%_srcd%\build\release\libmupdf-third.a" EXIT 1
COPY "%_srcd%\build\release\libmupdf.a" "%_outd%\libmupdf.a"
COPY "%_srcd%\build\release\libmupdf-third.a" "%_outd%\libmupdf-third.a"
EXIT 0
