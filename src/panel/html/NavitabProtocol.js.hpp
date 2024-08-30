class NavitabHttp {
    constructor(i) {
        this.imageBuffer = i;
        this.reqId = 1;
        this.lastResponseTime = 0;
        this.lastUpdateTime = 0;
        this.lastPositionTime = 0;
        this.imageLoading = false;
        this.imageReady = false;
        this.imageBuffer.addEventListener("load", () => {
            this.imageReady = true;
            this.imageLoading = false;
            this.lastResponseTime = Date.now();
        });
        this.imageBuffer.addEventListener("error", () => {
            this.imageLoading = false;
            this.lastResponseTime = Date.now();
        });
        this.lastTrafficTime = Date.now() + 5000;
        this.trafficLoading = false;
        this.trafficReady = false;
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
    mouseEvent(x,y,b) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/m?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + "&mb=" + b;
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
        let url = "http://127.0.0.1:26730/m?t=" + (this.reqId++) + "&mx=" + x + "&my=" + y + ((d<0) ? "&wu" : "&wd");
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    positionUpdate() {
        const lat = SimVar.GetSimVarValue("PLANE LATITUDE", "degree latitude");
        const lon = SimVar.GetSimVarValue("PLANE LONGITUDE", "degree longitude");
        const alt = SimVar.GetSimVarValue("PLANE ALTITUDE", "meters");
        const hdg = SimVar.GetSimVarValue("PLANE HEADING DEGREES TRUE", "degree");
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/m?t=" + (this.reqId++) + "&lt=" + lat.toFixed(4) + "&ln=" + lon.toFixed(4) + "&al=" + alt.toFixed(1) + "&hg=" + hdg.toFixed(0);
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    trafficUpdate(ta) {
        var self = this;
        let xhttp = new XMLHttpRequest();
        let url = "http://127.0.0.1:26730/m?t=" + (this.reqId++) + "&tr=";
        for (let i = 0; i < ta.length; i++) {
            const entry = ta[i];
            url = url + entry.lat.toFixed(4) + ',' + entry.lon.toFixed(4) + ',' + entry.alt.toFixed(1) + ',' + entry.heading.toFixed(0) + '_';
        }
        xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                self.onResponse(this);
            }
        };
        xhttp.open("GET", url);
        xhttp.send();
    }
    trafficRequest() {
        this.trafficLoading = true;
        Coherent.call("GET_AIR_TRAFFIC").then((obj) => {
            //console.log("TRAFFIC : " + obj.length + " airplanes found");
            this.trafficUpdate(obj);
            this.trafficLoading = false;
        });
    }
    update() {
        // called on each animation frame callback
        const now = Date.now();

#ifdef NAVITAB_MSFS_PANEL
        // TODO - move this into the server via the SDK plugin
        // get our position every 0.5 seconds
        if (((now - this.lastPositionTime) > 500)) {
            this.lastPositionTime = now;
            this.positionUpdate();
        }

        // get other traffic every 1 second
        if ((this.trafficLoading == false) && ((now - this.lastTrafficTime) > 1000)) {
            this.lastTrafficTime = now;
            this.trafficRequest();
        }
#endif

        // may want to throttle this back, currently loads images as fast as responses allow
        if ((this.imageReady == false) && (this.imageLoading == false)) {
            this.imageLoading = true;
            let url = "http://127.0.0.1:26730/f?t=" + (this.reqId++);
            this.imageBuffer.src = url;
        }

        // let the caller know if a new image is ready for drawing, or if connection is lost
        let ready = this.imageReady;
        this.imageReady = false;
        return [ready, ((now - this.lastResponseTime) > 3000)];
    }
}
