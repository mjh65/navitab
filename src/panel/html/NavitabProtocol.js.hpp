// NavitabProtocol class deals with communications with the panel server.
// The protocol uses http GET requests with various URL tags to represent
// different reported events. These are:
// RESIZE:  /e  w= h=
// MODE:    /e  m=
// TOOL:    /e  t=
// MOUSE:   /e  x= y= b= wu wd
// KEY:     /e  k=
// PING:    /p
// IMAGE:   /i
// All of these also have a ?z= parameter to avoid caching.
// Resize, mode, tool, mouse, key are all sent when triggered.
// Resize, mode, tool, mouse, key, status all return the current status in the response(or see image idea below)
// Ping is used to detect the server
// Image is requested continuously when connected.
// Status is requested after 1s of no previous request (could code it in an extra fake line of image?)
// Other TODOs:
// When starting: issue resize to check connection and try other port options if no response.
class NavitabProtocol {
    constructor() {
        this.portNum = 0;
        this.reqId = 1;
        this.failedLoads = 0;
        this.imageBuffer = null;
        this.canvas = null;
        this.imageLoading = false;
        this.statusLoading = false;
        this.codedStatus = "17432518176676146197";
    }
    // an image element for loading, and canvas for drawing and status extraction
    // this technique is not the cleanest, but is supported by MSFS
    setElements(b,c) {
        this.imageBuffer = b;
        this.canvas = c;
        this.imageBuffer.setAttribute("crossorigin", "anonymous");
        this.imageBuffer.addEventListener("load", () => {
            let ctx = this.canvas.getContext('2d');
            ctx.drawImage(this.imageBuffer, 0, 0);
            this.failedLoads = 0;
            this.imageLoading = false;
        });
        this.imageBuffer.addEventListener("error", () => {
            ++this.failedLoads;
            this.imageLoading = false;
        });
    }
    // set the server's port number
    setPort(p) {
        this.portNum = p;
        this.imageLoading = false;
        this.statusLoading = false;
        this.failedLoads = 0;
    }
    // decide if the connection has been lost (server died or shutdown)
    isConnected() {
        if (!this.portNum) return false;
        let active = (this.failedLoads < 4);
        if (!active) {
            if (this.portNum || this.imageLoading || this.statusLoading) {
                this.setPort(0);
            }
        }
        return active;
    }
    // generic function to send panel events to the server, failures are ignored
    sendMessage(url) {
        if (!this.portNum) return false;
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", url, true);
        xhttp.timeout = 1000;
        xhttp.send();
        return this.isConnected();
    }
    // report some mouse action over the canvas
    mouseEvent(x,y,b) {
        this.sendMessage("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&x=" + x + "&y=" + y + "&b=" + b);
    }
    // report scroll wheel movements
    wheelEvent(x,y,d) {
        this.sendMessage("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&x=" + x + "&y=" + y + ((d<0) ? "&wu" : "&wd"));
    }
    // report resizing of the panel
    reportCanvasSize(w,h) {
        console.log("Sending canvas size %d x %d", w, h);
        this.sendMessage("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&w=" + w + "&h=" + h);
    }
    // report click on a mode icon
    reportModeClick(m) {
        this.sendMessage("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&m=" + m);
    }
    // report click on a tool icon
    reportToolClick(t) {
        this.sendMessage("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&t=" + t);
    }
    // get the current status string
    getStatus() {
        const xhr = new XMLHttpRequest();
        const url = "http://127.0.0.1:" + this.portNum + "/s" + (this.reqId++);
        xhr.open("GET", url, true);
        xhr.onreadystatechange = () => {
            if (xhr.readyState === XMLHttpRequest.DONE) {
                const status = xhr.status;
                if (status === 0 || (status >= 200 && status < 400)) {
                    this.codedStatus = xhr.responseText;
                }
                this.statusLoading = false;
            }
        };
        xhr.send();
    }
    // get the next update from the server: an image for the canvas, and a status code
    pollServer() {
        if (!this.isConnected()) return "";
        if (!this.imageLoading) {
            this.imageLoading = true;
            const url = "http://127.0.0.1:" + this.portNum + "/i" + (this.reqId++);
            this.imageBuffer.src = url;
        }
        if (!this.statusLoading) {
            this.statusLoading = true;
            this.getStatus();
        }
        return this.codedStatus;
    }
}

// PortFinder sends pings to the potential server ports and reports the first connection it finds
class PortFinder {
    constructor(b,n) {
        this.basePort = b;
        this.topPort = b + n - 1;
        this.reqId = 1;
        this.linkDown();
    }
    linkDown() {
        this.portNumber = 0;
        this.nextPort = this.basePort;
        this.nextPoll = Date.now();
    }
    getPortNumber() {
        if (this.portNumber) return this.portNumber;
        if (Date.now() > this.nextPoll) {
            // create and send a ping request to the next port
            const xhr = new XMLHttpRequest();
            const url = "http://127.0.0.1:" + this.nextPort + "/p?z=" + (this.reqId++) + "&p=" + this.nextPort;
            xhr.open("GET", url, true);
            xhr.onreadystatechange = () => {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    // if there is a response find out which port it was sent to and use it
                    const u = xhr.responseURL;
                    const i = u.search("&p=");
                    if (i > 0) {
                        const p = u.substr(i + 3);
                        this.portNumber = parseInt(p);
                    }
                }
            };
            xhr.timeout = 1000;
            xhr.send();
            // prepare the port number for the next ping
            let port = this.nextPort + 1;
            if ((port < this.basePort) || (port > this.topPort)) {
                port = this.basePort;
            }
            this.nextPort = port;
            this.nextPoll = Date.now() + 250;
        }
        return 0;
    }
}
