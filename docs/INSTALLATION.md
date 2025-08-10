# Navitab Installation

Navitab is available in a number of configurations. This document describes
their use and how to install each one.

## Navitab for Microsoft Flight Simulator

Navitab for Microsoft Flight Simulator (MSFS) consists of an **In-game Panel** which
appears in the Toolbar, and a separate self-contained desktop (console) executable
called the **Panel Server** which does most of the work and streams the UI to the
**In-game Panel**.

### Installation

To install Navitab for MSFS copy the folder `navitab-ingamepanels-navitab` from the
`msfs` sub-folder into the `Community` folder of your MSFS installation. The location
of the MSFS `Community` folder will depend on the original MSFS installation.

This [article](https://helpdesk.aerosoft.com/hc/en-gb/articles/5023507568925-How-to-locate-the-Community-folder-in-Microsoft-Flight-Simulator)
may help if the `Community` folder is proving to be elusive.

To update Navitab it is recommended to remove the old folder before copying the new
one. Navitab does not store any working files in the installed folder.

To uninstall Navitab from MSFS simply remove the folder `navitab-ingamepanels-navitab`
from the MSFS `Community` folder. Navitab stores its working files separately from the
panel and server. To remove these files too refer to the final section of this document
to find the location and then delete this folder/directory.

### Operation

The **In-game Panel** provides the UI for the **Panel Server** which must be running
concurrently with the Flight Simulator. The **Panel Server** is an executable desktop
application `navitab_igps.exe` that can be found in the `PanelServer` folder in the
installed directory. It may be desirable to create a shortcut to this and put it
somewhere more convenient.

The **Panel Server** can be started before or after Flight Simulator. It is a console
application and will display status and logging information in text form as it is running.
The GUI for the **Panel Server** is displayed in the **In-game Panel** which will be found
in the Flight Simulator Toolbar. The **In-game Panel** works in desktop and VR modes.

When it is no longer needed the **Panel Server** can be stopped by closing its console
window, or by typing `exit` or `quit` into the console.

## Navitab for X-Plane

Navitab for X-Plane consists of a **plugin** that is loaded when X-Plane is started.

### Installation

To install Navitab for X-Plane copy the folder `Navitab` from the `xplane` sub-folder
into the `plugins` folder of your X-Plane installation. The `plugins` folder will be found
in the `Resources` folder which is at the top-level of the X-Plane installation folder.

To update Navitab it is recommended to remove the old folder before copying the new
one. Navitab does not store any working files in the installed folder.

To uninstall Navitab from X-Plane simply remove the folder `Navitab` from the `plugins` folder.
Navitab stores its working files separately from the plugin. To remove these files too
refer to the final section of this document to find the location and then delete this
folder/directory.

### Operation

When the X-Plane simulator is running Navitab can be enabled from the Plugins menu.
There will be a Navitab sub-menu with commands to show or hide the Navitab panel.
Navitab is supported in desktop and VR modes.

## Navitab Development Tools

If you are building Navitab from source then there will also be some Navitab variants
to assist with development and debugging.

### Navitab Desktop

This is a self-contained GLFW desktop application that simplifies GUI development
away from the simulator environment. A mock simulation interface is used, which can
be programmed if predictable testing is required. The executable can be found in the
`tools` folder. Working files are stored as described in the final section of this
document.

### Navitab Html/JS

This is a development system specifically for the MSFS in-game panel variant. As for
the MSFS installation there is a GUI sub-system and a desktop server. The desktop server
is called `navitab_http[.exe]` and will be found in the `tools` folder.

The Navitab GUI is implemented in html/Javascript, and the files will be found in the
`htdocs` folder. These files will need to be *served* to a standard web browser - I
have used the Servez application.

This environment is mainly intended for development and testing of the MSFS panel UI,
and the communications protocol between the panel and the server.

Working files are stored as described in the final section of this document.

## Navitab Working Files

Navitab stores all of its working files separately from the installable packages. The
location of these files varies according to the platform. The location of the working
files is shown in the About app on the main Navitab display. However if Navitab has
already been uninstalled, or is not working correctly then the working files can be
located by finding a folder/directory call Navitab in one of the following locations.

On Windows working files are stored in `%LOCALAPPDATA%\Navitab`
(failing that `%APPDATA%\Navitab`, failing that `%USERPROFILE%\Navitab`, failing that
`%TEMP%\Navitab`, or as a last resort `C:\Navitab`).

On Linux working files are stored in `~/.navitab` or `~/.config/navitab`.

On Mac working files are stored in `~/Library/Application Support/Navitab` (failing that
`${TMPDIR}/Navitab`, or `/tmp/Navitab`.

The working files are generally shared between all variations of Navitab that might run
on the platform, although in the normal case, this is likely to be only 1 version, or
possibly 2 where both Xplane and MSFS are installed on the same Windows system.

Preference, log files and databases are named to allow peaceful cohabitation. Other
*read-only* files are shared and available to all of the avitab variants.
