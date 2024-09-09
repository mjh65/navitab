body {
    margin: 0;
    padding: 0;
}
navitab-element #NavitabPanel {
    width: 100%;
    height: 100%;
    min-width: 400px;
    min-height: 344px;
    max-width: 1600px;
    max-height: 700px;
    display: flex;
}
navitab-element #AllParts {
    position: relative;
    width: 100vw;
    height: 100vh;
    background-color: DarkKhaki;
}
navitab-element #ToolbarPart {
    position: absolute;
    left: 0px;
    top: 0px;
    width: 100%;
    height: 24px;
    background-color: Chartreuse;
}
navitab-element #ToolbarStatus {
    position: absolute;
    left: 4px;
    top: 4px;
    color: red;
}
navitab-element #ToolbarTools {
    position: absolute;
    right: 4px;
    top: 0px;
    height: 24px;
    z-index: 1;
}
navitab-element .nTool:hover {
    background-color: white;
}
navitab-element #ModebarPart {
    position: absolute;
    left: 0px;
    top: 24px;
    width: 40px;
    margin: 0px;
    background: rgba(255, 255, 255, 0.2);
    z-index: 1;
}
navitab-element .nMode:hover {
    background-color: gray;
}
navitab-element .imageArea {
    position: absolute;
    left: 0px;
    top: 24px;
    width: 100%;
    height: calc(100% - 24px);
}
