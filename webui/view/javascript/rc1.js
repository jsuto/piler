function roundcube_browser() {
    var a = navigator;
    this.ver = parseFloat(a.appVersion);
    this.appver = a.appVersion;
    this.agent = a.userAgent;
    this.agent_lc = a.userAgent.toLowerCase();
    this.name = a.appName;
    this.vendor = a.vendor ? a.vendor : "";
    this.vendver = a.vendorSub ? parseFloat(a.vendorSub) : 0;
    this.product = a.product ? a.product : "";
    this.platform = String(a.platform).toLowerCase();
    this.lang = a.language ? a.language.substring(0, 2) : a.browserLanguage ? a.browserLanguage.substring(0, 2) : a.systemLanguage ? a.systemLanguage.substring(0, 2) : "en";
    this.win = this.platform.indexOf("win") >= 0;
    this.mac = this.platform.indexOf("mac") >= 0;
    this.linux = this.platform.indexOf("linux") >= 0;
    this.unix = this.platform.indexOf("unix") >= 0;
    this.dom = document.getElementById ? !0 : !1;
    this.dom2 = document.addEventListener && document.removeEventListener;
    this.ie4 = (this.ie = document.all && !window.opera) && !this.dom;
    this.ie5 = this.dom && this.appver.indexOf("MSIE 5") > 0;
    this.ie8 = this.dom && this.appver.indexOf("MSIE 8") > 0;
    this.ie7 = this.dom && this.appver.indexOf("MSIE 7") > 0;
    this.ie6 = this.dom && !this.ie8 && !this.ie7 && this.appver.indexOf("MSIE 6") > 0;
    this.mz = this.dom && this.ver >= 5;
    this.ns6 = (this.ns = this.ver < 5 && this.name == "Netscape" || this.ver >= 5 && this.vendor.indexOf("Netscape") >= 0) && parseInt(this.vendver) == 6;
    this.ns7 = this.ns && parseInt(this.vendver) == 7;
    this.chrome = this.agent_lc.indexOf("chrome") > 0;
    this.safari = !this.chrome && (this.agent_lc.indexOf("safari") > 0 || this.agent_lc.indexOf("applewebkit") > 0);
    this.konq = this.agent_lc.indexOf("konqueror") > 0;
    this.iphone = this.safari && this.agent_lc.indexOf("iphone") > 0;
    this.ipad = this.safari && this.agent_lc.indexOf("ipad") > 0;
    if ((this.opera = window.opera ? !0 : !1) && window.RegExp) this.vendver = /opera(\s|\/)([0-9\.]+)/.test(this.agent_lc) ? parseFloat(RegExp.$2) : -1;
    else if (this.chrome && window.RegExp) this.vendver = /chrome\/([0-9\.]+)/.test(this.agent_lc) ? parseFloat(RegExp.$1) : 0;
    else if (!this.vendver && this.safari) this.vendver = /(safari|applewebkit)\/([0-9]+)/.test(this.agent_lc) ? parseInt(RegExp.$2) : 0;
    else if (!this.vendver && this.mz || this.agent.indexOf("Camino") > 0) this.vendver = /rv:([0-9\.]+)/.test(this.agent) ? parseFloat(RegExp.$1) : 0;
    else if (this.ie && window.RegExp) this.vendver = /msie\s+([0-9\.]+)/.test(this.agent_lc) ? parseFloat(RegExp.$1) : 0;
    else if (this.konq && window.RegExp) this.vendver = /khtml\/([0-9\.]+)/.test(this.agent_lc) ? parseFloat(RegExp.$1) : 0;
    if (this.safari && /;\s+([a-z]{2})-[a-z]{2}\)/.test(this.agent_lc)) this.lang = RegExp.$1;
    this.dhtml = this.ie4 && this.win || this.ie5 || this.ie6 || this.ns4 || this.mz;
    this.vml = this.win && this.ie && this.dom && !this.opera;
    this.pngalpha = this.mz || this.opera && this.vendver >= 6 || this.ie && this.mac && this.vendver >= 5 || this.ie && this.win && this.vendver >= 5.5 || this.safari;
    this.opacity = this.mz || this.ie && this.vendver >= 5.5 && !this.opera || this.safari && this.vendver >= 100;
    this.cookies = a.cookieEnabled;
    this.xmlhttp_test = function () {
        var a = new Function("try{var o=new ActiveXObject('Microsoft.XMLHTTP');return true;}catch(err){return false;}");
        return this.xmlhttp = window.XMLHttpRequest || window.ActiveXObject && a()
    };
    this.set_html_class = function () {
        var a = " js";
        this.ie ? (a += " ie", this.ie5 ? a += " ie5" : this.ie6 ? a += " ie6" : this.ie7 ? a += " ie7" : this.ie8 && (a += " ie8")) : this.opera ? a += " opera" : this.konq ? a += " konqueror" : this.safari && (a += " safari");
        this.chrome ? a += " chrome" : this.iphone ? a += " iphone" : this.ipad ? a += " ipad" : this.ns6 ? a += " netscape6" : this.ns7 && (a += " netscape7");
        document.documentElement && (document.documentElement.className += a)
    }
}

function rcube_layer(a, b) {
    this.name = a;
    this.create = function (c) {
        var b = c.x ? c.x : 0,
            e = c.y ? c.y : 0,
            f = c.width,
            g = c.height,
            i = c.zindex,
            j = c.vis,
            c = c.parent,
            h = document.createElement("DIV");
        with(h) {
            a = this.name;
            with(style) position = "absolute", visibility = j ? j == 2 ? "inherit" : "visible" : "hidden", left = b + "px", top = e + "px", f && (width = f.toString().match(/\%$/) ? f : f + "px"), g && (height = g.toString().match(/\%$/) ? g : g + "px"), i && (zIndex = i)
        }
        c ? c.appendChild(h) : document.body.appendChild(h);
        this.elm = h
    };
    b != null ? (this.create(b), this.name = this.elm.id) : this.elm = document.getElementById(a);
    if (!this.elm) return !1;
    this.css = this.elm.style;
    this.event = this.elm;
    this.width = this.elm.offsetWidth;
    this.height = this.elm.offsetHeight;
    this.x = parseInt(this.elm.offsetLeft);
    this.y = parseInt(this.elm.offsetTop);
    this.visible = this.css.visibility == "visible" || this.css.visibility == "show" || this.css.visibility == "inherit" ? !0 : !1;
    this.move = function (a, b) {
        this.x = a;
        this.y = b;
        this.css.left = Math.round(this.x) + "px";
        this.css.top = Math.round(this.y) + "px"
    };
    this.resize = function (a, b) {
        this.css.width = a + "px";
        this.css.height = b + "px";
        this.width = a;
        this.height = b
    };
    this.show = function (a) {
        a == 1 ? (this.css.visibility = "visible", this.visible = !0) : a == 2 ? (this.css.visibility = "inherit", this.visible = !0) : (this.css.visibility = "hidden", this.visible = !1)
    };
    this.write = function (a) {
        this.elm.innerHTML = a
    }
}

var rcube_event = {
    get_target: function (a) {
        return (a = a || window.event) && a.target ? a.target : a.srcElement
    },
    get_keycode: function (a) {
        return (a = a || window.event) && a.keyCode ? a.keyCode : a && a.which ? a.which : 0
    },
    get_button: function (a) {
        return (a = a || window.event) && typeof a.button != "undefined" ? a.button : a && a.which ? a.which : 0
    },
    get_modifier: function (a) {
        var b = 0,
            a = a || window.event;
        if (bw.mac && a) return b += (a.metaKey && CONTROL_KEY) + (a.shiftKey && SHIFT_KEY), b;
        if (a) return b += (a.ctrlKey && CONTROL_KEY) + (a.shiftKey && SHIFT_KEY), b
    },
    get_mouse_pos: function (a) {
        if (!a) a = window.event;
        var b = a.pageX ? a.pageX : a.clientX,
            c = a.pageY ? a.pageY : a.clientY;
        document.body && document.all && (b += document.body.scrollLeft, c += document.body.scrollTop);
        a._offset && (b += a._offset.left, c += a._offset.top);
        return {
            x: b,
            y: c
        }
    },
    add_listener: function (a) {
        if (a.object && a.method) {
            if (!a.element) a.element = document;
            if (!a.object._rc_events) a.object._rc_events = [];
            var b = a.event + "*" + a.method;
            a.object._rc_events[b] || (a.object._rc_events[b] = function (c) {
                return a.object[a.method](c)
            });
            a.element.addEventListener ? a.element.addEventListener(a.event,
            a.object._rc_events[b], !1) : a.element.attachEvent ? (a.element.detachEvent("on" + a.event, a.object._rc_events[b]), a.element.attachEvent("on" + a.event, a.object._rc_events[b])) : a.element["on" + a.event] = a.object._rc_events[b]
        }
    },
    remove_listener: function (a) {
        if (!a.element) a.element = document;
        var b = a.event + "*" + a.method;
        a.object && a.object._rc_events && a.object._rc_events[b] && (a.element.removeEventListener ? a.element.removeEventListener(a.event, a.object._rc_events[b], !1) : a.element.detachEvent ? a.element.detachEvent("on" + a.event, a.object._rc_events[b]) : a.element["on" + a.event] = null)
    },
    cancel: function (a) {
        a = a ? a : window.event;
        a.preventDefault && a.preventDefault();
        a.stopPropagation && a.stopPropagation();
        a.cancelBubble = !0;
        return a.returnValue = !1
    },
    touchevent: function (a) {
        return {
            pageX: a.pageX,
            pageY: a.pageY,
            offsetX: a.pageX - a.target.offsetLeft,
            offsetY: a.pageY - a.target.offsetTop,
            target: a.target,
            istouch: !0
        }
    }
};

function setCookie(a, b, c, d, e, f) {
    a = a + "=" + escape(b) + (c ? "; expires=" + c.toGMTString() : "") + (d ? "; path=" + d : "") + (e ? "; domain=" + e : "") + (f ? "; secure" : "");
    document.cookie = a
}

function getCookie(a) {
    var b = document.cookie;
    a += "=";
    var c = b.indexOf("; " + a);
    if (c == -1) {
        if (c = b.indexOf(a), c != 0) return null
    } else c += 2;
    var d = document.cookie.indexOf(";", c);
    if (d == -1) d = b.length;
    return unescape(b.substring(c + a.length, d))
}


var bw = new roundcube_browser;

roundcube_browser.prototype.set_cookie = setCookie;
roundcube_browser.prototype.get_cookie = getCookie;

