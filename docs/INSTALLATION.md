# Navitab Installation

Navitab is available in a number of configurations. This document describes
the use and how to install each one.

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

To uninstall Navitab from MSFS simply remove the folder `navitab-ingamepanels-navitab`
from the MSFS `Community` folder.

To update Navitab it is recommended to remove the old folder before copying the new
one. Navitab does not store any working files in the installed folder.

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

To uninstall Navitab from X-Plane simply remove the folder `Navitab` from the `plugins` folder.

To update Navitab it is recommended to remove the old folder before copying the new
one. Navitab does not store any working files in the installed folder.

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
`tools` folder.

### Navitab Html/JS

This is a development system specifically for the MSFS in-game panel variant. As for
the MSFS installation there is a GUI sub-system and a desktop server. The desktop server
is called `navitab_http[.exe]` and will be found in the `tools` folder.

The Navitab GUI is implemented in html/Javascript, and the files will be found in the
`htdocs` folder. These files will need to be *served* to a standard web browser - I
have used the Servez application.

This environment is mainly intended for development and testing of the MSFS panel UI,
and the communications protocol between the panel and the server.

