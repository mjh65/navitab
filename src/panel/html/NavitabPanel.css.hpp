aviators-tablet #NavitabPanel {
    width: 100%;
    height: 100%;
    min-width: 400px;
    min-height: 344px;
    max-width: 1600px;
    max-height: 700px;
    display: flex;
}
aviators-tablet #AllParts {
    position: relative;
    width: 100%;
    height: 100%;
    background-color: DarkKhaki;
}
aviators-tablet #ToolbarPart {
    position: absolute;
    left: 0px;
    top: 0px;
    width: 100%;
    height: 24px;
    background-color: Chartreuse;
}
aviators-tablet #ToolbarStatus {
    position: absolute;
    left: 4px;
    top: 4px;
}
aviators-tablet #ToolbarTools {
    position: absolute;
    right: 4px;
    top: 0px;
    height: 24px;
    background-color: Chartreuse;
    z-index: 1;
}
aviators-tablet #ModebarPart {
    position: absolute;
    left: 0px;
    top: 24px;
    width: 40px;
    margin: 0px;
    background: rgba(255, 255, 255, 0.2);
    z-index: 1;
}
aviators-tablet #Canvas {
    position: absolute;
    left: 0px;
    top: 24px;
    width: 100%;
    height: calc(100% - 24px);
}
aviators-tablet #ScratchArea {
    display: none;
    width: 100%;
    height: 0%;
}
aviators-tablet #ImageBuffer {
    display: none;
}
