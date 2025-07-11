@ECHO ON
:: Build the MuPDF libraries using Visual Studio (MSBuild).

:: Args - srcdir, outdir, config
IF [%1] EQU [] EXIT 1
IF [%2] EQU [] EXIT 1
IF [%3] EQU [] EXIT 1

:: Normalise the directory paths. CD is tolerant to / seperator, but not all commands are
CD "%2"
set "_outd=%CD%"
CD "%1\platform\win32"
set "_srcd=%CD%"
set _cfg=%3

IF EXIST "%_outd%\libmupdf.lib" DEL "%_outd%\libmupdf.lib"
IF EXIST "%_outd%\libthirdparty.lib" DEL "%_outd%\libthirdparty.lib"

:: TODO - VsWhere should be used to find MSBuild executable.

MSBuild.exe mupdf.sln /t:Build /p:Configuration=%_cfg%,PlatformToolset=v143 /m:8

IF %ERRORLEVEL% NEQ 0 EXIT 1
IF NOT EXIST "%_srcd%\%_cfg%\libmupdf.lib" EXIT 1
IF NOT EXIST "%_srcd%\%_cfg%\libthirdparty.lib" EXIT 1
COPY "%_srcd%\%_cfg%\libmupdf.lib" "%_outd%\mupdf.lib"
COPY "%_srcd%\%_cfg%\libthirdparty.lib" "%_outd%\mupdf-third.lib"
EXIT 0
