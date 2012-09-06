function rcube_webmail() {
    this.env = {};
    this.labels = {};
    this.buttons = {};
    this.buttons_sel = {};
    this.gui_objects = {};
    this.gui_containers = {};
    this.commands = {};
    this.command_handlers = {};
    this.onloads = [];
    this.messages = {};
    this.ref = "rcmail";
    var j = this;
    this.dblclick_time = 500;
    this.message_time = 2E3;
    this.identifier_expr = RegExp("[^0-9a-z-_]", "gi");
    this.mimetypes = "text/plain,text/html,text/xml,image/jpeg,image/gif,image/png,application/x-javascript,application/pdf,application/x-shockwave-flash".split(",");
    this.env.keep_alive = 60;
    this.env.request_timeout = 180;
    this.env.draft_autosave = 0;
    this.env.comm_path = "./";
    this.env.blankpage = "program/blank.gif";
    $.ajaxSetup({
        cache: !1,
        error: function (a, b, d) {
            j.http_error(a, b, d)
        },
        beforeSend: function (a) {
            a.setRequestHeader("X-Roundcube-Request", j.env.request_token)
        }
    });
    this.set_env = function (a, b) {
        if (a != null && typeof a == "object" && !b) for (var d in a) this.env[d] = a[d];
        else this.env[a] = b
    };
    this.add_label = function (a, b) {
        this.labels[a] = b
    };
    this.register_button = function (a, b, d, e, f, g) {
        this.buttons[a] || (this.buttons[a] = []);
        b = {
            id: b,
            type: d
        };
        if (e) b.act = e;
        if (f) b.sel = f;
        if (g) b.over = g;
        this.buttons[a].push(b)
    };
    this.gui_object = function (a, b) {
        this.gui_objects[a] = b
    };
    this.gui_container = function (a, b) {
        this.gui_containers[a] = b
    };
    this.add_element = function (a, b) {
        this.gui_containers[b] && this.gui_containers[b].jquery && this.gui_containers[b].append(a)
    };
    this.register_command = function (a, b, d) {
        this.command_handlers[a] = b;
        d && this.enable_command(a, !0)
    };
    this.add_onload = function (a) {
        this.onloads.push(a)
    };
    this.init = function () {
        var a = this;
        this.task = this.env.task;
        if (!bw.dom || !bw.xmlhttp_test()) this.goto_url("error", "_code=0x199");
        else {
            for (var b in this.gui_containers) this.gui_containers[b] = $("#" + this.gui_containers[b]);
            for (b in this.gui_objects) this.gui_objects[b] = rcube_find_object(this.gui_objects[b]);

            this.loaded = !0;
            this.pending_message && this.display_message(this.pending_message[0], this.pending_message[1]);
            if (this.gui_objects.folderlist) this.gui_containers.foldertray = $(this.gui_objects.folderlist);
            /*this.triggerEvent("init", {
                task: this.task,
                action: this.env.action
            });*/
            for (var d in this.onloads) if (typeof this.onloads[d] == "string") eval(this.onloads[d]);
            else if (typeof this.onloads[d] == "function") this.onloads[d]();
            //this.start_keepalive()
        }
    };

}
