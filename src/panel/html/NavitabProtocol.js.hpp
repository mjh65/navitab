#if 0
// NavitabProtocol class deals with communications with the panel server.
// The protocol uses http GET requests with various URL tags to represent
// different reported events. These are:
// PING:    /p
// RESIZE:  /r  w= h=
// MODE:    /a  q=
// TOOL:    /t  q=
// MOUSE:   /m  x= y= b= wu wd
// KEY:     /k  c=
// STATUS:  /s
// IMAGE:   /i
// All of these also have a ?t= parameter to increment a timestamp.
// Resize, mode, tool, mouse, key are all sent when triggered.
// Resize, mode, tool, mouse, key, status all return the current status in the response(or see image idea below)
// Image is requested continuously when connected.
// Status is requested after 1s of no previous request (could code it in an extra fake line of image?)
// Other TODOs:
// When starting: issue resize to check connection and try other port options if no response.
#endif

class NavitabProtocol {
    constructor() {
        this.reqId = 1;
        this.portNum = 0;
        this.lastResponseTime = 0;
        this.lastUpdateTime = 0;
        this.imageLoading = false;
        this.lastUrl = "favicon.svg";
#ifdef NAVITAB_MSFS_PANEL
        let listener = RegisterViewListener('JS_LISTENER_MAPS', () => {
            listener.trigger('JS_BIND_BINGMAP', 'navitab' + Date.now(), false);
        });
#endif
    }
    onResponse(resp) {
        //console.log("onResponse " + resp.responseText);
        this.lastResponseTime = Date.now();
        // TODO - extract server status info from resp.responseText
    }
    ping(p) {
        console.log("Sending ping to port %d", p);
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:" + p + "/p?t=" + (this.reqId++);
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                console.log("Ping response on port %d", p);
                self.portNum = p;
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    mouseEvent(x,y,b) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:" + this.portNum + "/m?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + "&mb=" + b;
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    wheelEvent(x,y,d) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:" + this.portNum + "/m?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + ((d<0) ? "&wu" : "&wd");
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    setCanvasSize(w,h) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:" + this.portNum + "/r?t=" + (this.reqId++) + "&w=" + w + "&h=" + h;
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    canvasUrl() {
        if (!this.imageLoading) {
            var self = this;
            var newImg = new Image;
            this.imageLoading = true;
            let url = "http://127.0.0.1:" + this.portNum + "/i" + (this.reqId++);
            newImg.onload = function() {
                console.log("Image loaded");
                self.imageLoading = false;
                self.lastResponseTime = Date.now();
                self.lastUrl = this.src;
            }
            newImg.onerror = function() {
                console.log("Image error");
                self.imageLoading = false;
                self.lastResponseTime = Date.now();

            }
            newImg.src = url;
        }
        return this.lastUrl;
    }
    isConnected() {
        let lastRespDelta = Date.now() - this.lastResponseTime;
        let active = lastRespDelta < 3000;
        if (this.imageLoading && !active) { this.imageLoading = false; }
        return active;
    }
}
