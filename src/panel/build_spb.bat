@ECHO OFF
@REM This script uses the MSFS SDK's 'fspackagetool' to create the packaging metafiles for the Navitab in-game panel

SET _pkgname_=mjh65-ingamepanels-navitab

DEL /Q /S "%CD%\%_pkgname_%"
DEL /Q /S "%CD%\fspt"
MD "%CD%\fspt\Packages\%_pkgname_%"
TYPE NUL > "%CD%\fspt\Packages\%_pkgname_%\layout.json"
XCOPY /I /S "%1\spb\*.*" "%CD%\fspt\"
XCOPY /I /S "%1\html_ui" "%CD%\fspt\PackageSources\html_ui"
XCOPY /I %CD"%\fspt\PackageSources\html_ui\icons\toolbar\*.svg" "%CD%\fspt\PackageSources\html_ui\Textures\Menu\toolbar\"

"%MSFS_SDK%\Tools\bin\fspackagetool.exe" "%CD%\fspt\%_pkgname_%.xml" -nopause

IF NOT EXIST "%CD%\fspt\Packages\%_pkgname_%\Build\%_pkgname_%.spb" EXIT 1
IF NOT EXIST "%CD%\fspt\Packages\%_pkgname_%\manifest.json" EXIT 1
IF NOT EXIST "%CD%\fspt\Packages\%_pkgname_%\layout.json" EXIT 1

XCOPY /I "%CD%\fspt\Packages\%_pkgname_%\Build\%_pkgname_%.spb" "%CD%\%_pkgname_%\InGamePanels"
XCOPY /I "%CD%\fspt\Packages\%_pkgname_%\manifest.json" "%CD%\%_pkgname_%"
XCOPY /I "%CD%\fspt\Packages\%_pkgname_%\layout.json" "%CD%\%_pkgname_%"

EXIT 0
