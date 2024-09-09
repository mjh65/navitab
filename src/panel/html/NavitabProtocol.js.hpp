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
        this.canvas = null;
        this.imageLoading = false;
        this.codedStatus = "17432518176676146197";
    }
    setCanvas(c) {
        this.canvas = c;
    }
    // set the server's port number
    setPort(p) {
        this.portNum = p;
        this.imageLoading = false;
        this.failedLoads = 0;
    }
    // decide if the connection has been lost (server died or shutdown)
    isConnected() {
        if (!this.portNum) return false;
        let active = (this.failedLoads < 4);
        if (!active) {
            if (this.portNum || this.imageLoading) {
                this.setPort(0);
            }
        }
        return active;
    }
    // generic send request function
    sendRequest(url) {
        if (!this.portNum) return false;
        var self = this;
        let xhttp = new XMLHttpRequest();
        xhttp.open("GET", url);
        xhttp.timeout = 1000;
        xhttp.send();
        return this.isConnected();
    }
    // report some mouse action over the canvas
    mouseEvent(x,y,b) {
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&x=" + x + "&y=" + y + "&b=" + b);
    }
    // report scroll wheel movements
    wheelEvent(x,y,d) {
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&x=" + x + "&y=" + y + ((d<0) ? "&wu" : "&wd"));
    }
    // report resizing of the panel
    reportCanvasSize(w,h) {
        console.log("Sending canvas size %d x %d", w, h);
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&w=" + w + "&h=" + h);
    }
    // report click on a mode icon
    reportModeClick(m) {
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&m=" + m);
    }
    // report click on a tool icon
    reportToolClick(t) {
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/e?z=" + (this.reqId++) + "&t=" + t);
    }
    // callback when image request gets a response
    imageResponse(resp) {
        if (resp.readyState == 4) {
            if (resp.status == 200) {
                this.failedLoads = 0;
                const sh = resp.getResponseHeader("Navitab-Status");
                if (sh) this.codedStatus = sh;
                let bitmappromise = createImageBitmap(resp.response);
                bitmappromise.then(bitmap => {
                    this.canvas.getContext('2d').drawImage(bitmap, 0, 0);
                    this.imageLoading = false;
                });
            } else {
                ++this.failedLoads;
                this.imageLoading = false;
            }
        }
    }
    // get the next update from the server: an image for the canvas, and a status code
    pollServer() {
        if (!this.isConnected()) return "";
        if (!this.imageLoading) {
            this.imageLoading = true;
            let self = this;
            let xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                self.imageResponse(this);
            };
            let url = "http://127.0.0.1:" + this.portNum + "/i" + (this.reqId++);
            xhttp.open("GET", url);
            xhttp.timeout = 1000;
            xhttp.responseType = "blob";
            xhttp.send();
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
            var self = this;
            let xhttp = new XMLHttpRequest();
            let url = "http://127.0.0.1:" + this.nextPort + "/p?z=" + (this.reqId++) + "&p=" + this.nextPort;
            xhttp.onreadystatechange = function() {
                if (this.readyState == 2) {
                    let u = this.responseURL;
                    let i = u.search("&p=");
                    let p = u.substr(i + 3);
                    self.portNumber = parseInt(p);
                }
            };
            xhttp.open("GET", url);
            xhttp.timeout = 1000;
            xhttp.send();
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
