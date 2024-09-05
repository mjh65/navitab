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
// TemplateElement is an MSFS class which we need to provide for inheritance purposes
// TODO - or do we? why not just conditionally compile the inheritance declaration of NavitabElement??
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

// NavitabStatus holds the latest status and commands from the server
class NavitabStatus {
    constructor() {
        this.wallClock = "13:23:52";
        this.fps = 18;
        this.zuluClock = "17:42:33";
        this.longitude = -3.324;
        this.latitude = 56.197;
    }
    format() {
        return this.wallClock + " | " + this.fps + " | " + this.zuluClock + " | " + this.longitude + "," + this.latitude;
    }
}


class NavitabElement extends TemplateElement {
    constructor() {
        super(...arguments);
        this.panelActive = false;
        this.ingameUi = null;
        this.statusElem = null;
        this.canvas = null;
        this.server = null;
        this.currentImage = null;
        this.connected = true;
        this.mouseDown = false;
        this.status = new NavitabStatus();
        this.server = new NavitabProtocol();
        this.finder = new PortFinder(26730, 20);
        this.resizePending = 0;
        this.nextStatus = Date.now() + 1000;
    }
    connectedCallback() {
        // this is when the panel is connected to the simulation,
        // nothing to do with the panel server!
        super.connectedCallback();

        var self = this;
        this.ingameUi = this.querySelector('ingame-ui');
        this.statusElem = document.getElementById("ToolbarStatus");
        this.canvas = document.getElementById("Canvas");
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
    disconnectedCallback() {
        super.disconnectedCallback();
    }
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
    drawCanvas(i) {
        //console.log("Latest image size is %d x %d", i.width, i.height);
        let ctx = this.canvas.getContext("2d");
        ctx.drawImage(i, 0, 0, i.width, i.height, 0, 0, this.canvas.width, this.canvas.height);
        this.currentImage = i;
        if (this.connected && ((this.canvas.width != i.width) || (this.canvas.height != i.height)) && !this.resizePending) {
            this.resizePending = Date.now() + 500;
        }
    }
    flightLoop() {
        if (Date.now() > this.nextStatus) {
            this.statusElem.textContent = this.status.format(); // TODO - make this part of the onResponse callback
            if (!this.server.getStatus()) {
                console.log("Connection to panel server has been lost");
                this.connected = false;
                this.finder.linkDown();
                this.statusElem.textContent = "Waiting for connection to Navitab panel server";
            }
            this.nextStatus = Date.now() + 1000;
            return;
        }
        let latest = this.server.getLatestImage();
        if (latest && (latest !== this.currentImage)) {
            this.drawCanvas(latest);
        }
    }
    findServer() {
        let p = this.finder.getPortNumber();
        if (p) {
            console.log("Connected to panel server on port %d", p);
            this.connected = true;
            this.server.setPort(p);
            this.nextStatus = Date.now();
            this.resizePending = Date.now();
        } else {
            this.drawCanvas(this.server.getLatestImage());       
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
