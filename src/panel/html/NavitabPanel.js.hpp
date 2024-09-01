#if 0
// NavitabElement class is the custom HTMLElement derivative for the Navitab panel.
// It is responsible for the top-level Navitab UI, including mode(app) selection,
// tool clicks, and passing on mouse events to the panel server.
#endif

var NavitabIsLoaded = false;
document.addEventListener('beforeunload', function () {
    NavitabIsLoaded = false;
}, false);

#ifdef NAVITAB_MOCK_WWW
class TemplateElement extends HTMLElement {
    constructor() {
        super(...arguments);
    }
    connectedCallback() {
    }
    disconnectedCallback() {
    }
}
#endif

class NavitabStatus {
    constructor() {
        this.wallClock = 0;
        this.fps = 18;
        this.zuluClock = 0;
        this.longitude = 0;
        this.latitude = 0;
    }
    format() {
        return "13:23:52 | 17fps | 17:42:33Z | -3.324,56.197";
    }
}

class NavitabElement extends TemplateElement {
    constructor() {
        super(...arguments);
        this.panelActive = false;
        this.ingameUi = null;
        this.statusElem = null;
        this.canvas = null;
        this.imageBuffer = null;
        this.server = null;
        this.serverPort = 0;
        this.mouseDown = false;
        this.resizePending = Date.now();
        this.pingPending = Date.now();
    }
    connectedCallback() {
        // this is when the panel is connected to the simulation,
        // nothing to do with the panel server!
        super.connectedCallback();

        var self = this;
        this.ingameUi = this.querySelector('ingame-ui');
        this.statusElem = document.getElementById("ToolbarStatus");
        this.statusText = new NavitabStatus();
        this.canvas = document.getElementById("Canvas");
        this.imageBuffer = document.getElementById("ImageBuffer");
        this.server = new NavitabProtocol(this.imageBuffer);

        if (this.ingameUi) {
            this.ingameUi.addEventListener("panelActive", (e) => {
                //console.log('NavitabElement::panelActive');
                self.panelActive = true;
                let updateLoop = () => {
                    if (window["IsDestroying"] === true) {
                        return;
                    }
                    if (!NavitabIsLoaded) {
                        return;
                    }
                    this.flightLoop();
                    requestAnimationFrame(updateLoop);
                };
                NavitabIsLoaded = true;
                requestAnimationFrame(updateLoop);
            });
            this.ingameUi.addEventListener("panelInactive", (e) => {
                //console.log('NavitabElement::panelInactive');
                self.panelActive = false;
            });
            this.ingameUi.addEventListener("OnResize", () => {
                self.resizePending = Date.now() + 1000;
            });
        }

        if (this.canvas) {
            this.canvas.addEventListener("mousemove", (e) => {
                if (this.mouseDown) {
                    this.server.mouseEvent(e.offsetX, e.offsetY , 1);
                }
            });
            this.canvas.addEventListener("mousedown", (e) => {
                this.mouseDown = true;
                this.server.mouseEvent(e.offsetX, e.offsetY , 1);
            });
            this.canvas.addEventListener("mouseup", (e) => {
                this.mouseDown = false;
                this.server.mouseEvent(e.offsetX, e.offsetY , 0);
            });
            this.canvas.addEventListener("wheel", (e) => {
                this.server.wheelEvent(e.offsetX, e.offsetY, e.deltaY);
            });
        }
    }
    disconnectedCallback() {
        console.log('NavitabElement::disconnectedCallback()');
        super.disconnectedCallback();
    }
    updateCanvas() {
        let ctx = this.canvas.getContext("2d");
        ctx.drawImage(this.imageBuffer, 0, 0);
    }

    flightLoop() {
        if (this.server.isConnected()) {
            this.statusElem.textContent = this.statusText.format();
            if (Date.now() > this.resizePending) {
                const rect = this.canvas.parentNode.getBoundingClientRect();
                console.log("Resize pending %d x %d", rect.width, rect.height);
                this.canvas.width = rect.width;
                this.canvas.height = rect.height;
                this.server.setCanvasSize(rect.width, rect.height);
                this.resizePending = Date.now() + 100000; // will fire again, but not for a long time!
            }
            else if (this.server.hasImageReady()) {
                this.updateCanvas();
            }
        } else if (Date.now() > this.pingPending) {
            this.statusElem.textContent = "Waiting for connection to Navitab panel server";
            let port = this.serverPort + 1;
            if ((port < 26730) || (port >= 26750)) {
                port = 26730;
            }
            this.serverPort = port;
            this.pingPending = Date.now() + 500;
            this.resizePending = Date.now();
            this.server.ping(port);
        }
    }
}
#ifdef NAVITAB_MSFS_PANEL
// this call is too early for the generic htdocs version, see the html
window.customElements.define("navitab-element", NavitabElement);
checkAutoload();
#endif
#ifdef NAVITAB_MOCK_WWW
function startPanel() {
    console.log("START PANEL");
    window.customElements.define("navitab-element", NavitabElement);
    let ui = document.querySelector('ingame-ui');
    window.addEventListener("resize", (event) => {
            const resize = new Event("OnResize");
            ui.dispatchEvent(resize);
        });
    window.addEventListener("unload", () => {
            const inactive = new Event("panelInactive");
            ui.dispatchEvent(inactive);
        });
    const active = new Event("panelActive");
    ui.dispatchEvent(active);
    const setsize = new Event("OnResize");
    ui.dispatchEvent(setsize);
}
#endif
