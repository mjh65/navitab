
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

class NavitabElement extends TemplateElement {
    constructor() {
        super(...arguments);
        console.log('NavitabElement::NavitabElement()');
        this.panelActive = false;
        this.ingameUi = null;
        this.canvas = null;
        this.canvasX = 800;
        this.canvasY = 480;
        this.imageBuffer = null;
        this.mouseDown = false;
        this.resizePending = Date.now();
    }
    connectedCallback() {
        console.log('NavitabElement::connectedCallback()');
        super.connectedCallback();

        var self = this;
        this.ingameUi = this.querySelector('ingame-ui');
        this.canvas = document.getElementById("Canvas");
        this.imageBuffer = document.getElementById("ImageBuffer");
        this.http = new NavitabHttp(this.imageBuffer);

        if (this.ingameUi) {
            this.ingameUi.addEventListener("panelActive", (e) => {
                console.log('NavitabElement::panelActive');
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
                console.log('NavitabElement::panelInactive');
                self.panelActive = false;
            });
            this.ingameUi.addEventListener("OnResize", () => {
                self.resizePending = Date.now() + 1000;
            });
        }

        if (this.canvas) {
            this.canvas.addEventListener("mousemove", (e) => {
                if (this.mouseDown) {
                    this.http.mouseEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), 1);
                }
            });
            this.canvas.addEventListener("mousedown", (e) => {
                this.mouseDown = true;
                this.http.mouseEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), 1);
            });
            this.canvas.addEventListener("mouseup", (e) => {
                this.mouseDown = false;
                this.http.mouseEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), 0);
            });
            this.canvas.addEventListener("wheel", (e) => {
                this.http.wheelEvent(Math.round(800 * e.offsetX / this.canvasX), Math.round(480 * e.offsetY / this.canvasY), e.deltaY);
            });
        }
    }
    disconnectedCallback() {
        console.log('NavitabElement::disconnectedCallback()');
        super.disconnectedCallback();
    }
    updateCanvas() {
        let ctx = this.canvas.getContext("2d");
        ctx.drawImage(this.imageBuffer, 0, 0, 800, 480, 0, 0, this.canvasX, this.canvasY);
    }
    noConnection(me) {
        // TODO - scale this to fit canvas
        let ctx = this.canvas.getContext("2d");
        ctx.fillStyle = "red";
        ctx.strokeStyle = "red";
        ctx.lineWidth = 20;
        ctx.beginPath();
        ctx.arc(400, 160, 100, 0, 2 * Math.PI);
        ctx.moveTo(470, 90);
        ctx.lineTo(330, 230);
        ctx.stroke();
        ctx.font = "36px Arial";
        ctx.fillStyle = "black";
        ctx.fillText("!no response from Navitab panel server!", 70, 350);
    }

    flightLoop() {
        if (Date.now() > this.resizePending) {
            console.log("Resize pending");

            this.resizePending = Date.now() + 100000; // will fire but not for a long time!
        }
        let res = this.http.update();
        if (res[1]) {
            this.noConnection();
        } else if (res[0]) {
            this.updateCanvas();
        }
    }
}
#ifdef NAVITAB_MSFS_PANEL
// this call is too early for the generic htdocs version, see the html
window.customElements.define("aviators-tablet", NavitabElement);
checkAutoload();
#endif
#ifdef NAVITAB_MOCK_WWW
function startPanel() {
    console.log("START PANEL");
    window.customElements.define("aviators-tablet", NavitabElement);
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
