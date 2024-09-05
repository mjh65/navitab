// NavitabProtocol class deals with communications with the panel server.
// The protocol uses http GET requests with various URL tags to represent
// different reported events. These are:
// RESIZE:  /r  w= h=
// MODE:    /a  q=
// TOOL:    /t  q=
// MOUSE:   /m  x= y= b= wu wd
// KEY:     /k  c=
// STATUS:  /s
// PING:    /p
// IMAGE:   /i
// All of these also have a ?t= parameter to increment a timestamp.
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
        this.logo = new Image();
        this.logo.src = "favicon.svg";
        this.currentImage = this.logo;
        this.imageLoading = false;
        // we use two image buffers for background loading
        this.imageLoader0 = this.makeImageLoader();
        this.imageLoader1 = this.makeImageLoader();
    }
    // create a new image buffer, and assign callbacks
    makeImageLoader() {
        let i = new Image();
        let self = this;
        i.setAttribute('crossOrigin','anonymous');
        i.onload = function() {
            self.failedLoads = 0;
            self.currentImage = this;
            self.imageLoading = false;
        }
        i.onerror = function() {
            console.log("Image loader failure");
            ++self.failedLoads;
            self.imageLoading = false;
        }
        return i;
    }
    // set the server's port number
    setPort(p) {
        this.portNum = p;
        this.failedLoads = 0;
        this.imageLoading = false;
    }
    // decide if the connection has been lost (server died or shutdown)
    isConnected() {
        if (!this.portNum) return false;
        let active = (this.failedLoads < 4);
        if (!active) {
            if (this.portNum || this.imageLoading) {
                this.setPort(0);
                this.currentImage = this.logo;
            }
        }
        return active;
    }
    // deal with responses from event and other requests sent
    onResponse(resp) {
        if (resp.readyState == 4) {
            if (resp.status == 200) {
                this.failedLoads = 0;
                // TODO extract status info from headers and pass to panel class
            } else {
                ++this.failedLoads;
            }
        }
    }
    // generic send request function
    sendRequest(url) {
        if (!this.portNum) return false;
        var self = this;
        let xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
            self.onResponse(this);
        };
        xhttp.open("GET", url);
        xhttp.timeout = 1000;
        xhttp.send();
        return this.isConnected();
    }
    // get an updated status string
    getStatus() {
        return this.sendRequest("http://127.0.0.1:" + this.portNum + "/s?t=" + (this.reqId++));
    }
    // report some mouse action over the canvas
    mouseEvent(x,y,b) {
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/m?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + "&mb=" + b);
    }
    // report scroll wheel movements
    wheelEvent(x,y,d) {
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/m?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + ((d<0) ? "&wu" : "&wd"));
    }
    // report resizing of the panel
    reportCanvasSize(w,h) {
        console.log("Sending canvas size %d x %d", w, h);
        this.sendRequest("http://127.0.0.1:" + this.portNum + "/r?t=" + (this.reqId++) + "&w=" + w + "&h=" + h);
    }
    // return the most recently acquired image (or the logo if the server connection was lost)
    getLatestImage() {
        if (this.portNum && !this.imageLoading) {
            let url = "http://127.0.0.1:" + this.portNum + "/i" + (this.reqId++);
            this.imageLoading = true;
            if (this.currentImage === this.imageLoader0) {
                this.imageLoader1.src = url;
            } else {
                this.imageLoader0.src = url;
            }
        }
        return this.currentImage;
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
            let url = "http://127.0.0.1:" + this.nextPort + "/s?q=" + (this.reqId++) + "&p=" + this.nextPort;
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
