<!DOCTYPE html>
<html>
<head>
    <meta charset="utf-8" />
#ifdef NAVITAB_MSFS_PANEL
    <link rel="stylesheet" href="/SCSS/common.css" />
#endif
    <link rel="stylesheet" href="NavitabPanel.css" />
#ifdef NAVITAB_MSFS_PANEL
    <script type="text/javascript" src="/JS/coherent.js"></script>
    <script type="text/javascript" src="/JS/common.js"></script>
    <script type="text/javascript" src="/JS/simvar.js"></script>
    <script type="text/javascript" src="/JS/dataStorage.js"></script>
    <script type="text/javascript" src="/JS/buttons.js"></script>
    <script type="text/javascript" src="/JS/Services/ToolBarPanels.js"></script>
    <link rel="import" href="/templates/ingameUi/ingameUi.html" />
    <link rel="import" href="/templates/ingameUiHeader/ingameUiHeader.html" />
#endif
#ifdef NAVITAB_MOCK_WWW
    <link rel="icon" href="favicon.svg" type="image/svg+xml"/>
#endif
    <script type="text/javascript" src="NavitabProtocol.js"></script>
    <script type="text/javascript" src="NavitabPanel.js"></script>
</head>
<body id="Navitab" class="border-box">
    <navitab-element>
        <ingame-ui id="NavitabPanel" panel-id="PANEL_NAVITAB" class="ingameUiFrame panelInvisible" title="Navitab" content-fit="true">
            <div id="AllParts">
                <div id="ToolbarPart">
                    <div id="ToolbarRect">
                        <code id="ToolbarStatus">13:23:52 | 17fps | 17:42:33Z | -3.324,56.197</code>
                    </div>
                    <div id="ToolbarTools">
                        <img id="ToolCentre" src="tool_centre.svg" alt="Centre" width="24px" height="24px">
                        <img id="ToolZoomI" src="tool_zoomin.svg" alt="Zoom in" width="24px" height="24px">
                        <img id="ToolZoomO" src="tool_zoomout.svg" alt="Zoom out" width="24px" height="24px">
                        <img id="ToolRotateL" src="tool_rotateleft.svg" alt="Rotate left" width="24px" height="24px">
                        <img id="ToolRotateR" src="tool_rotateright.svg" alt="Rotate right" width="24px" height="24px">
                        <img id="ToolTop" src="tool_top.svg" alt="Go to top" width="24px" height="24px">
                        <img id="ToolUp" src="tool_up.svg" alt="Up" width="24px" height="24px">
                        <img id="ToolDown" src="tool_down.svg" alt="Down" width="24px" height="24px">
                        <img id="ToolBottom" src="tool_bottom.svg" alt="Go to bottom" width="24px" height="24px">
                        <img id="ToolFirst" src="tool_first.svg" alt="First" width="24px" height="24px">
                        <img id="ToolLeft" src="tool_left.svg" alt="Left" width="24px" height="24px">
                        <img id="ToolRight" src="tool_right.svg" alt="Right" width="24px" height="24px">
                        <img id="ToolLast" src="tool_last.svg" alt="Last" width="24px" height="24px">
                        <img id="ToolCancel" src="tool_cancel.svg" alt="Cancel" width="24px" height="24px">
                        <img id="ToolStop" src="tool_stop.svg" alt="Stop" width="24px" height="24px">
                        <img id="ToolCog" src="tool_cog.svg" alt="Settings" width="24px" height="24px">
                    </div>
                </div>
                <div id="ModebarPart">
                    <img id="ModeAbout" src="mode_about.svg" alt="About" width="40px" height="40px">
                    <img id="ModeMap" src="mode_map.svg" alt="Map" width="40px" height="40px">
                    <img id="ModeAirport" src="mode_airport.svg" alt="Airport" width="40px" height="40px">
                    <img id="ModeRoute" src="mode_route.svg" alt="Route" width="40px" height="40px">
                    <img id="ModeDocs" src="mode_docs.svg" alt="Docs" width="40px" height="40px">
                    <img id="ModeSettings" src="mode_settings.svg" alt="Settings" width="40px" height="40px">
                    <img id="ModeDoodler" src="mode_doodler.svg" alt="Doodler" width="40px" height="40px">
                    <img id="ModeKeypad" src="mode_keypad.svg" alt="Keypad" width="40px" height="24px">
                </div>
                <img id="Canvas" src="favicon.svg"></canvas>
            </div>
        </ingame-ui>
    </navitab-element>
#ifdef NAVITAB_MOCK_WWW
    <script> (function() { startPanel(); })(); </script>
#endif
</body>
</html>
