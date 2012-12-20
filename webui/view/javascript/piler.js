
var attachment_types = ["word", "excel", "powerpoint", "pdf", "compressed", "text", "odf", "vcard", "image", "audio", "video", "flash", "other"];
var count = 0;
var expsrc = 0;

var current_message_serial = 1;
var current_messages = new Array();

function getXMLHttp() {
   var XMLHttp = null;

   if(window.XMLHttpRequest) {
      try {
         XMLHttp = new XMLHttpRequest();
      } catch(e) { }
   }
   else if(window.ActiveXObject) {
      try {
         XMLHttp = new ActiveXObject("Msxml2.XMLHTTP");
      } catch (e) {
         try {
            XMLHttp = new ActiveXObject("Microsoft.XMLHTTP");
         } catch(e) { }
      }
   }

   return XMLHttp;
}


function loadHealth() {

   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) document.getElementById('A1').innerHTML = http.responseText;
         else alert("Problem retrieving XML data:" + http.statusText);
      }
   }

   http.open("GET", health_worker_url, true);
   http.send(null);

   document.body.style.cursor = 'default';
}


function load_ajax_url(url) {

   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) document.getElementById('A1').innerHTML = http.responseText;
         else alert("Problem retrieving XML data:" + http.statusText);
      }
   }

   http.open("GET", url, true);
   http.send(null);

   document.body.style.cursor = 'default';

}


function load_search_results(url, params, page) {
   current_message_serial = 0;
   current_messages = new Array();

   document.getElementById('messagelistcontainer').innerHTML = '<img src="/view/theme/default/images/spinner.gif" id="spinner" alt="spinner" />';

   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.open("POST", url, true);

   params = params + "&page=" + page;

   http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
   http.setRequestHeader("Content-length", params.length);
   http.setRequestHeader("Connection", "close");


   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) {
            document.getElementById('mailcontframe').innerHTML = http.responseText;

            fill_current_messages_array();
         }
         else {
            alert("Problem retrieving XML data:" + http.statusText);
         }
      }
   }

   http.send(params);
}


function load_saved_search_terms(url) {
   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) document.getElementById('mailcontframe').innerHTML = http.responseText;
         else alert("Problem retrieving XML data:" + http.statusText);
      }
   }

   http.open("GET", url, true);
   http.send(null);

   document.body.style.cursor = 'default';

}


function load_audit_results(url, params, page) {

   document.getElementById('AS').innerHTML = '<img src="/view/theme/default/images/spinner.gif" id="spinner" alt="spinner" />';

   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.open("POST", url, true);

   params = params + "&page=" + page;

   http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
   http.setRequestHeader("Content-length", params.length);
   http.setRequestHeader("Connection", "close");


   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) {
            document.getElementById('AS').innerHTML = http.responseText;
         }
         else {
            alert("Problem retrieving XML data:" + http.statusText);
         }
      }
   }

   http.send(params);
}


function load_url_with_get(url, id) {
   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200){
            document.getElementById(id).innerHTML = http.responseText;
         }
         else alert("Problem retrieving XML data:" + http.statusText);
      }
   }

   http.open("GET", url, true);
   http.send(null);

   document.body.style.cursor = 'default';


}


function send_ajax_post_request(url, params) {
   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.open("POST", url, true);

   http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
   http.setRequestHeader("Content-length", params.length);
   http.setRequestHeader("Connection", "close");


   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) {
         }
         else {
            alert("Problem retrieving XML data:" + http.statusText);
         }
      }
   }

   http.send(params);
}


function add_message_reference_to_form(s) {
   e = document.getElementById('ref');
   if(e) e.value = s;
}


function assemble_search_term(n) {
   var prefix = '';
   var data = '';
   var attachment_type = '';
   var e;
   var folders = '';
   var extra_folders = '';
   var childNodeArray = new Array();

   e = document.getElementById('prefix');
   if(e && e.value) { prefix = e.value; }

   e = document.getElementById(prefix + 'searchtype');
   if(e && e.value) { data = data + "searchtype=" + e.value; }

   e = document.getElementById(prefix + '_search');
   if(e && e.value) { data = data + "&search=" + e.value; }

   e = document.getElementById(prefix + 'from');
   if(e && e.value) { data = data + "&from=" + e.value; }

   e = document.getElementById(prefix + 'to');
   if(e && e.value) { data = data + "&to=" + e.value; }

   e = document.getElementById(prefix + 'subject');
   if(e && e.value) { data = data + "&subject=" + e.value; }

   e = document.getElementById(prefix + 'body');
   if(e && e.value) { data = data + "&body=" + e.value; }

   for(i=0; i<=n; i++) {
      var a = 'key' + i;
      var b = 'val' + i;

      e = document.getElementById(b);

      if(e && document.getElementById(b).value) {
         if(data) data = data + "&f%5B%5D=" + document.getElementById(a).value + "&v%5B%5D=" + encodeURI(document.getElementById(b).value);
         else data = "f%5B%5D=" + document.getElementById(a).value + "&v%5B%5D=" + encodeURI(document.getElementById(b).value);

      }

   }

   e = document.getElementById('date1');
   if(e && e.value) { data = data + "&date1=" + e.value; }

   e = document.getElementById('date2');
   if(e && e.value) { data = data + "&date2=" + e.value; }

   e = document.getElementById(prefix + 'direction');
   if(e && e.value) { data = data + "&direction=" + e.value; }

   e = document.getElementById(prefix + 'action');
   if(e && e.value) { data = data + "&action=" + e.value; }

   for(i=0; i<attachment_types.length; i++) {
      e = document.getElementById('attachment_type_' + attachment_types[i]);
      if(e && e.checked == 1) { attachment_type += '|' + attachment_types[i]; }
   }

   if(attachment_type) { data = data + "&attachment_type=" + attachment_type.substring(1,attachment_type.length); }

   e = document.getElementById(prefix + 'tag');
   if(e && e.value) { data = data + "&tag=" + e.value; }

   e = document.getElementById(prefix + 'note');
   if(e && e.value) { data = data + "&note=" + e.value; }

   e = document.getElementById(prefix + 'has_attachment');
   if(e && e.checked == 1) { data = data + "&has_attachment=1"; }

   e = document.getElementById(prefix + 'ref');
   if(e && e.value) { data = data + "&ref=" + e.value; }

   e = document.getElementById(prefix + 'sort');
   if(e && e.value) { data = data + "&sort=" + e.value; }

   e = document.getElementById(prefix + 'order');
   if(e && e.value) { data = data + "&order=" + e.value; }



   a = document.getElementById('folders');
   if(a) { childNodeArray = a.getElementsByTagName('*'); }
   if(childNodeArray) {
      for(i=0; i<childNodeArray.length; i++) {
         b = childNodeArray[i];
         if(b.name && b.name.substring(0, 7) == 'folder_' && b.checked) {
            folders = folders + "+" + b.name.substring(7);
         }

         if(b.name && b.name.substring(0, 13) == 'extra_folder_' && b.checked) {
            extra_folders = extra_folders + "+" + b.name.substring(13);
         }

      }
   }

   if(folders) {
      folders = folders.substring(1);
      data = data + "&folders=" + folders;
   }

   if(extra_folders) {
      extra_folders = extra_folders.substring(1);
      data = data + "&extra_folders=" + extra_folders;
   }

   //alert("data: " + data);

   return data;
}


function fix_search_order(sort, order) {
   var a;

   a = document.getElementById('xsort');
   if(a) a.value = sort;

   a = document.getElementById('sort');
   a.value = sort;

   a = document.getElementById('xorder');
   if(a) a.value = order;

   a = document.getElementById('order');
   a.value = order;
}


function fixup_save_post_action_url() {
   var a = document.getElementById('searchterm');

   var s = a.options[a.selectedIndex].value;

   if(s.substring(0,17) == "searchtype=simple") {
      document.savesearchform.action = "search.php";
   }
   else if(s.substring(0,17) == "searchtype=expert") {
      document.savesearchform.action = "expert.php";
   }
}


function reset_simple_form() {
   var a;

   a = document.getElementById('date1'); a.value = '';
   a = document.getElementById('date2'); a.value = '';

   a = document.getElementById('xfrom'); a.value = '';
   a = document.getElementById('xto'); a.value = '';
   a = document.getElementById('xsubject'); a.value = '';
   a = document.getElementById('xbody'); a.value = '';
   a = document.getElementById('xtag'); a.value = '';
   a = document.getElementById('xnote'); a.value = '';
   a = document.getElementById('xhas_attachment'); a.checked = 0;

   a = document.getElementById('ref');
   if(a) { a.value = ''; }
}


function reset_adv_form(n) {
   var a;

   for(i=n;i>0;i--) { removeme('myid' + i); }

   a = document.getElementById('date1'); a.value = '';
   a = document.getElementById('date2'); a.value = '';

   for(i=0; i<attachment_types.length; i++) {
      e = document.getElementById('attachment_type_' + attachment_types[i]);
      if(e) { e.checked = 0; }
   }

   a = document.getElementById('direction');
   if(a) { a[0].selected = 1; }

   a = document.getElementById('key0');
   if(a) { a[0].selected = 1; }

   a = document.getElementById('val0');
   if(a) { a.value = ''; }

   a = document.getElementById('ref');
   if(a) { a.value = ''; }

}


function addblock(key, val, t_sender, t_recipient, t_subject, t_body) {
   var from_selected = from_domain_selected = to_selected = to_domain_selected = subject_selected = body_selected = '';

   if(key == 'from') { from_selected = ' selected="selected"'; }
   if(key == 'from_domain') { from_domain_selected = ' selected="selected"'; }
   if(key == 'to') { to_selected = ' selected="selected"'; }
   if(key == 'to_domain') { to_domain_selected = ' selected="selected"'; }
   if(key == 'subject') { subject_selected = ' selected="selected"'; }
   if(key == 'body') { body_selected = ' selected="selected"'; }

   count++;

   var divid = 'myid' + count;

   var newdiv = document.createElement('div');

   newdiv.setAttribute('id', divid);

   newdiv.innerHTML = '<div id="' + divid + '" class="row" style="border: 0px solid green; float:left;">' +
                      '   <div class="cell1s">' +
                      '      <select name="f[]" class="advselect" id="key' + count + '">' +
                      '         <option value="from"' + from_selected + '>' + t_sender + '</option>' +
                      '         <option value="to"' + to_selected + '>' + t_recipient + '</option>' +
                      '         <option value="subject"' + subject_selected + '>' + t_subject + '</option>' +
                      '         <option value="body"' + body_selected + '>' + t_body + '</option>' +
                      '      </select>' + 
                      '   </div>' +
                      '   <div class="cell2">' +
                      '      <input type="text" class="advtext" name="v[]" id="val' + count + '" value="' + val + '" /> <input type="button" class="short" value="+" onclick="javascript:addblock(\'\', \'\', \'' + t_sender + '\', \'' + t_recipient + '\', \'' + t_subject + '\', \'' + t_body + '\'); return false;" />' +
                      '      <input type="button" class="short" value="-" onclick="javascript:removeme(\'' + divid + '\'); return false;" />' + 
                      '   </div>' +
                      '</div>';


   var a = document.getElementById('myid');
   a.appendChild(newdiv);
}


function addauditblock(key, val, t_user, t_ipaddr, t_ref) {
   var user_selected = ipaddr_selected = ref_selected = '';

   if(key == 'user') { user_selected = ' selected="selected"'; }
   if(key == 'ipaddr') { ipaddr_selected = ' selected="selected"'; }
   if(key == 'ref') { ref_selected = ' selected="selected"'; }

   count++;

   var divid = 'myid' + count;

   var newdiv = document.createElement('div');

   newdiv.setAttribute('id', divid);

   newdiv.innerHTML = '<div id="' + divid + '" class="row" style="border: 0px solid green; float:left;">' +
                      '   <div class="cell1s">' +
                      '      <select name="f[]" class="advselect" id="key' + count + '">' +
                      '         <option value="user"' + user_selected + '>' + t_user + '</option>' +
                      '         <option value="ipaddr"' + ipaddr_selected + '>' + t_ipaddr + '</option>' +
                      '         <option value="ref"' + ref_selected + '>' + t_ref + '</option>' +
                      '      </select>' +
                      '   </div>' +
                      '   <div class="cell2">' +
                      '      <input type="text" class="advtext" name="v[]" id="val' + count + '" value="' + val + '" /> <input type="button" class="short" value="+" onclick="javascript:addauditblock(\'\', \'\', \'' + t_user + '\', \'' + t_ipaddr + '\', \'' + t_ref + '\'); return false;" />' +
                      '      <input type="button" class="short" value="-" onclick="javascript:removeme(\'' + divid + '\'); return false;" />' +
                      '   </div>' +
                      '</div>';


   var a = document.getElementById('myid');
   a.appendChild(newdiv);
}


function removeme(id) {
   var a = document.getElementById('myid');
   if(a.childNodes.length <= 3) return;

   var ele = document.getElementById(id);
   if(ele) ele.parentNode.removeChild(ele);
}


function tag_search_results(url){
   var idlist = assemble_idlist();

   var tag_value = document.getElementById('tag_value').value;

   if(idlist) {
      params = "idlist=" + idlist + "&tag=" + encodeURI(tag_value);

      var http = getXMLHttp();

      if(http == null) { alert("Error creating XMLHttpRequest"); return; }

      http.onreadystatechange = function() {
         if(http.readyState == 4) {
            if(http.status != 200) alert("Problem retrieving XML data:" + http.statusText);
         }
      }

      http.open("POST", url, true);

      http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
      http.setRequestHeader("Content-length", params.length);
      http.setRequestHeader("Connection", "close");

      http.send(params);
   }
}


function assemble_idlist() {
   var a = document.getElementById('results');
   var idlist = "";

   len = a.childNodes.length;

   for(i=0; i<a.childNodes.length; i++) {

      if(a.childNodes[i].nodeName == "DIV" && a.childNodes[i].id.substring(0, 2) == "e_") {

         id = a.childNodes[i].id.substring(2,1000);

         b = document.getElementById('r_' + id);

         if(b.checked == true) {

            if(idlist) { idlist += "," + id; }
            else { idlist = id; }
         }
      }
   }

   return idlist;
}


function restore_selected_emails(url) {

   var idlist = assemble_idlist();

   if(idlist) {
      var http = getXMLHttp();

      if(http == null) { alert("Error creating XMLHttpRequest"); return; }

      http.open("POST", url, true);

      params = "idlist=" + idlist + "&download=0";

      http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
      http.setRequestHeader("Content-length", params.length);
      http.setRequestHeader("Connection", "close");


      http.onreadystatechange = function() {
         if(http.readyState == 4) {
            if(http.status == 200) {
            }
            else {
               alert("Problem retrieving XML data:" + http.statusText);
            }
         }
      }

      http.send(params);
   }

}


function download_selected_emails(url) {
   var hiddenField;
   var idlist = assemble_idlist();

   if(idlist) {

      var form = document.createElement("form");

      form.setAttribute("method", "post");
      form.setAttribute("action", url);
      form.setAttribute("name", "download");

      hiddenField = document.createElement("input");

      hiddenField.setAttribute("type", "hidden");
      hiddenField.setAttribute("name", "download");
      hiddenField.setAttribute("value", "1");
      form.appendChild(hiddenField);

      hiddenField = document.createElement("input");

      hiddenField.setAttribute("type", "hidden");
      hiddenField.setAttribute("name", "idlist");
      hiddenField.setAttribute("value", idlist);
      form.appendChild(hiddenField);

      document.body.appendChild(form);
      form.submit();
   }
}


function toggle_bulk_check() {
   var isChecked = document.getElementById('bulkcheck').value;

   var a = document.getElementById('results');

   len = a.childNodes.length;

   for(i=0; i<a.childNodes.length; i++) {

      if(a.childNodes[i].nodeName == "DIV" && a.childNodes[i].id.substring(0, 2) == "e_") {

         id = a.childNodes[i].id.substring(2,1000);

         b = document.getElementById('r_' + id);

         if(isChecked == 1) { b.checked = 0; }
         else { b.checked = 1; }
      }
   }

   if(isChecked == 1) {
      document.getElementById('bulkcheck').checked = 0;
      document.getElementById('bulkcheck').value = 0;
   }
   else {
      document.getElementById('bulkcheck').checked = 1;
      document.getElementById('bulkcheck').value = 1;
   }
}


function append_email_from_slider(id, value) {
   var prefix = '\n';

   a = opener.document.getElementById('email');
   if(a && a.value == '') prefix = '';

   a.value += prefix + value;
}


function fill_current_messages_array() {
   var a = document.getElementById('results');
   j = 1;

   len = a.childNodes.length;

   for(i=0; i<a.childNodes.length; i++) {

      if(a.childNodes[i].nodeName == "DIV" && a.childNodes[i].id.substring(0, 2) == "e_") {

         id = a.childNodes[i].id.substring(2,1000);

         current_messages[j] = id; j++;
      }
   }

}


$(document).ready(function() {
   $.datepicker.setDefaults($.datepicker.regional[piler_ui_lang]);
   $("#date1").datepicker( { dateFormat: 'yy-mm-dd' } );
   $("#date2").datepicker( { dateFormat: 'yy-mm-dd' } );

  });



  $(function() {

    $("#s_piler_email").autocomplete({
        source: email_search_url,
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';
                   var a = document.getElementById("email");

                   if(a && a.value == '') prefix = '';

                   $('#email').val($('#email').val() + prefix + ui.item.value);
                }

                ui.item.value = '';
        }
    });

    $("#s_piler_group").autocomplete({
        source: group_search_url,
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';
                   var a = document.getElementById("group");

                   if(a && a.value == '') prefix = '';

                   $('#group').val($('#group').val() + prefix + ui.item.value);
                }

                ui.item.value = '';
        }
    });

    $("#s_piler_folder").autocomplete({
        source: folder_search_url,
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';
                   var a = document.getElementById("folder");

                   if(a && a.value == '') prefix = '';

                   $('#folder').val($('#folder').val() + prefix + ui.item.value);
                }

                ui.item.value = '';
        }
    });

  });


function toggle_hint(id, s, focus) {

   if(focus == 1){
      if(document.getElementById(id).value == s) document.getElementById(id).value = '';
   }
   else {
      if(document.getElementById(id).value == '') document.getElementById(id).value = s;
   }
}


function show_message(id, msg, timeout){
   document.getElementById(id).innerHTML = msg;
   document.getElementById(id).style.display = '';
   setTimeout(function() { document.getElementById(id).style.display = 'none'; }, timeout*1000);
}


function set_scroll_position(current_message_serial) {
   var objDiv = document.getElementById("messagelistcontainer");
   if(objDiv) {
      objDiv.scrollTop = current_message_serial*17 - 30;
   }
}


function highlight_message_by_position(pos) {
   var a;

   for(i=1; i<=current_messages.length; i++) {
      a = document.getElementById('e_' + current_messages[i]);
      if(a) {
         if(i % 2) a.className = 'resultrow odd';
         else a.className = 'resultrow';
      }
   }

   load_url_with_get(message_loader_url + current_messages[pos], 'mailpreviewframe');

   a = document.getElementById('e_' + current_messages[pos]);

   if(a){ a.className = 'resultrow selected'; }

   set_scroll_position(pos);

}


function move_message_list_scrollbar(direction) {

   if(direction == 'down') {
      if(current_message_serial < current_messages.length-1) current_message_serial++;
   }
   else {
      if(current_message_serial > 1) current_message_serial--;
   }

   highlight_message_by_position(current_message_serial);
}


function copy_message_to_folder(folder_id, id, copied) {
   if(id <= 0) { return 0; }

   params = "folder_id=" + folder_id + "&id=" + id;

   send_ajax_post_request(folder_copy_url, params);

   show_message('messagebox1', '<p>' + copied + '</p>', 0.85);

   current_message_id = 0;

}


function open_folder(id) {
   var a;

   a  = document.getElementById('fldr_' + id);
   a.style.display = '';

   a  = document.getElementById('fldr_collapse_' + id);
   a.style.display = '';

   a = document.getElementById('fldr_open_' + id);
   a.style.display = 'none';

}


function close_folder(id) {
   var a;

   a  = document.getElementById('fldr_' + id);
   a.style.display = 'none';

   a  = document.getElementById('fldr_collapse_' + id);
   a.style.display = 'none';

   a = document.getElementById('fldr_open_' + id);
   a.style.display = '';
}


function assemble_recipient_list() {
   var a = document.getElementById('restorebox');
   var emails = '';

   for(i=0; i<a.childNodes.length; i++) {

      if(a.childNodes[i].id && a.childNodes[i].id.substring(0, 5) == "rcpt_") {
         var e = document.getElementById(a.childNodes[i].id);
         if(e && e.checked == 1) {

            email = a.childNodes[i].id.substring(5,1000);

            if(emails) { emails += ' ' + email; } else { emails = email; }
         }

      }
   }

   return encodeURI(emails);
}


