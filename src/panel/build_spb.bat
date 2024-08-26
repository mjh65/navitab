@ECHO OFF
:: This script uses the MSFS SDK's 'fspackagetool' to create the packaging metafiles for the Navitab in-game panel

SET _pkgname_=mjh65-ingamepanels-navitab
SET "_currentdir_=%CD%"

:: Remove any previous build files
IF EXIST "%CD%\%_pkgname_%" RD /Q /S "%CD%\%_pkgname_%"
IF EXIST "%CD%\fspt" RD /Q /S "%CD%\fspt"

:: Create and populate the build folder for the fspackagetool
MD "%CD%\fspt\Packages\%_pkgname_%"
TYPE NUL > "%CD%\fspt\Packages\%_pkgname_%\layout.json"
XCOPY /Q /I /S "%1\spb\*.*" "%CD%\fspt\"
MD "%CD%\fspt\PackageSources\html_ui\icons\toolbar"
XCOPY /Q /I /S "%1\svg\*.*" "%CD%\fspt\PackageSources\html_ui\icons\toolbar"
MD "%CD%\fspt\PackageSources\html_ui\Textures\Menu\toolbar"
XCOPY /Q /I /S "%1\svg\*.*" "%CD%\fspt\PackageSources\html_ui\Textures\Menu\toolbar"
MD "%CD%\fspt\PackageSources\html_ui\InGamePanels\NavitabPanel"
XCOPY /Q /I /S "%CD%\panel\*.*" "%CD%\fspt\PackageSources\html_ui\InGamePanels\NavitabPanel"
XCOPY /Q /I /S "%1\..\..\res\svg\*.svg" "%CD%\fspt\PackageSources\html_ui\InGamePanels\NavitabPanel"

:: Run the MSFS SDK's packaging tool - takes some time, and hangs if MSFS is running
"%MSFS_SDK%\Tools\bin\fspackagetool.exe" "%CD%\fspt\%_pkgname_%.xml" -nopause
CD "%_currentdir_%"

:: Check that the required files have been created, and fail if not
IF NOT EXIST "%CD%\fspt\Packages\%_pkgname_%\Build\%_pkgname_%.spb" EXIT 1
IF NOT EXIST "%CD%\fspt\Packages\%_pkgname_%\manifest.json" EXIT 1
IF NOT EXIST "%CD%\fspt\Packages\%_pkgname_%\layout.json" EXIT 1

:: Edit the paths in layout.json to remove the extra nonsense

:: Copy the build products from the fspackagetool's build directory to where CMake expects them
MD "%CD%\%_pkgname_%\InGamePanels"
XCOPY /Q /I "%CD%\fspt\Packages\%_pkgname_%\Build\%_pkgname_%.spb" "%CD%\%_pkgname_%\InGamePanels"
XCOPY /Q /I "%CD%\fspt\Packages\%_pkgname_%\manifest.json" "%CD%\%_pkgname_%"
:: Use JREPL.BAT (Dave Benham, dostips.com) to fix the paths in layout.com during the copy
"%1/jrepl.bat" "Build/%_pkgname_%" "InGamePanels/%_pkgname_%" < "%CD%\fspt\Packages\%_pkgname_%\layout.json" | "%1/jrepl.bat" "Build/" "" > "%CD%\%_pkgname_%\layout.json"
XCOPY /Q /I /S "%CD%\fspt\Packages\%_pkgname_%\Build\html_ui" "%CD%\%_pkgname_%\html_ui"

EXIT 0
