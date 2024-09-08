// NavitabElement class is the custom HTMLElement derivative for the Navitab panel.
// It is responsible for the top-level Navitab UI, including mode(app) selection,
// tool clicks, and passing on mouse events to the panel server.

var NavitabIsLoaded = false;
document.addEventListener('beforeunload', function () {
    NavitabIsLoaded = false;
}, false);

// NavitabStatus holds the latest status and commands from the server
class NavitabStatus {
    constructor() {
        this.wallClock = "13:23:52";
        this.fps = "18";
        this.zuluClock = "17:42:33";
        this.longitude = "-3.324";
        this.latitude = "56.197";
        this.nextUpdate = Date.now();
    }
    update(cs) {
        if (Date.now() > this.nextUpdate) {
            const d = new Date();
            this.wallClock = d.toTimeString().substr(0,8);
            this.zuluClock = cs.substr(0,2) + ":" + cs.substr(2,2) + ":" + cs.substr(4,2);
            this.fps = cs.substr(6,2);
            const lg = (parseInt(cs.substr(8,6)) / 1000) - 180;
            this.longitude = lg.toFixed(3);
            const lt = (parseInt(cs.substr(14,6)) / 1000) - 90;
            this.latitude = lt.toFixed(3);
            this.nextUpdate = Date.now() + 1000;
        }
    }
    format() {
        return this.wallClock + " | " + this.fps + "fps | " + this.zuluClock + "Z | " + this.longitude + "," + this.latitude;
    }
}

#ifdef NAVITAB_MOCK_WWW
// TemplateElement is an MSFS class which is derived from HTMLElement
// We can just skip a generation for non-MSFS purposes
#define TemplateElement HTMLElement
#endif

class NavitabElement extends TemplateElement {
    constructor() {
        super(...arguments);
        this.panelActive = false;
        this.ingameUi = null;
        this.statusElem = null;
        this.canvas = null;
        this.noServerImage = null;
        this.connected = true;
        this.mouseDown = false;
        this.status = new NavitabStatus();
        this.server = new NavitabProtocol();
        this.finder = new PortFinder(26730, 20);
        this.resizePending = 0;
    }
    connectedCallback() {
        // this is when the panel is connected to the simulation,
        // nothing to do with the panel server!
#ifdef NAVITAB_MSFS_PANEL
        super.connectedCallback();
#endif

        var self = this;
        this.ingameUi = this.querySelector('ingame-ui');
        this.statusElem = document.getElementById("ToolbarStatus");
        this.statusElem.textContent = "Waiting for connection to Navitab panel server";
        this.canvas = document.getElementById("Canvas");
        this.server.setCanvas(this.canvas);
        this.noServerImage = document.getElementById("NoServer");
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
                    this.checkResizeCanvas();
                    if (this.connected) {
                        this.flightLoop();
                    } else {
                        this.findServer();
                    }
                    requestAnimationFrame(updateLoop);
                };
                NavitabIsLoaded = true;
                requestAnimationFrame(updateLoop);
            });
            this.ingameUi.addEventListener("panelInactive", (e) => {
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
#ifdef NAVITAB_MSFS_PANEL
    disconnectedCallback() {
        super.disconnectedCallback();
    }
#endif
    checkResizeCanvas() {
        if (this.resizePending && (Date.now() > this.resizePending)) {
            const rect = this.canvas.parentNode.getBoundingClientRect();
            const w = rect.width;
            const h = rect.height;
            if ((this.canvas.width != w) || (this.canvas.height != h)) {
                console.log("Canvas has been resized to %d x %d", w, h);
                this.canvas.width = w;
                this.canvas.height = h;
            }
            this.server.reportCanvasSize(w, h);
            this.resizePending = 0;
        }
    }
    flightLoop() {
        const cs = this.server.pollServer();
        if (cs) {
            this.status.update(cs);
            this.statusElem.textContent = this.status.format();
        } else {
            console.log("Connection to panel server has been lost");
            this.connected = false;
            this.finder.linkDown();
            this.noServerImage.style.display = "block";
            this.statusElem.textContent = "Waiting for connection to Navitab panel server";
        }
    }
    findServer() {
        const p = this.finder.getPortNumber();
        if (p) {
            console.log("Connected to panel server on port %d", p);
            this.connected = true;
            this.server.setPort(p);
            this.noServerImage.style.display = "none";
            this.resizePending = Date.now();
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
