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
                <img id="ImageBuffer" class="canvasArea">
                <canvas id="Canvas" class="canvasArea"></canvas>
                <img id="NoServer" class="canvasArea" src="favicon.svg" alt="No server">
            </div>
        </ingame-ui>
    </navitab-element>
#ifdef NAVITAB_MOCK_WWW
    <script> (function() { startPanel(); })(); </script>
#endif
</body>
</html>
