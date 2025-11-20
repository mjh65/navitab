// NavitabElement class is the custom HTMLElement derivative for the Navitab panel.
// It is responsible for passing on mouse events to the panel server.

var NavitabIsLoaded = false;
document.addEventListener('beforeunload', function () {
    NavitabIsLoaded = false;
}, false);

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
        this.imageBuffer = null;
        this.canvas = null;
        this.noServerImage = null;
        this.connected = true;
        this.mouseDown = false;
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

        this.ingameUi = this.querySelector('ingame-ui');
        this.imageBuffer = document.getElementById("ImageBuffer");
        this.imageBuffer.style.display = "none";
        this.canvas = document.getElementById("Canvas");
        this.server.setElements(this.imageBuffer, this.canvas);
        this.noServerImage = document.getElementById("NoServer");
        if (this.ingameUi) {
            this.ingameUi.addEventListener("panelActive", (e) => {
                //console.log('NavitabElement::panelActive');
                this.panelActive = true;
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
                this.panelActive = false;
            });
            this.ingameUi.addEventListener("OnResize", () => {
                this.resizePending = Date.now() + 1000;
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
    lostServer() {
        console.log("Connection to panel server has been lost");
        this.connected = false;
        this.finder.linkDown();
        this.noServerImage.style.display = "block";
    }
    flightLoop() {
        if (this.server.pollServer() != 0) {
            this.lostServer();
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
