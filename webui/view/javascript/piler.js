
var attachment_types = ["word", "excel", "powerpoint", "pdf", "compressed", "text", "odf", "vcard", "image", "audio", "video", "flash", "other"];
var count = 0;
var expsrc = 0;


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

   document.getElementById('AS').innerHTML = '<div id="resultscontainer" class="empty"><img src="/view/theme/default/images/spinner.gif" id="spinner" alt="spinner" /></div>';

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

            $(".messagelink").colorbox({iframe:true, width:"80%", height:"80%" });

         }
         else {
            alert("Problem retrieving XML data:" + http.statusText);
         }
      }
   }

   http.send(params);
}


function send_ajax_post_request(url, params, id) {
   var http = getXMLHttp();

   if(http == null) { alert("Error creating XMLHttpRequest"); return; }

   http.open("POST", url, true);

   http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
   http.setRequestHeader("Content-length", params.length);
   http.setRequestHeader("Connection", "close");


   http.onreadystatechange = function() {
      if(http.readyState == 4) {
         if(http.status == 200) {
            document.getElementById(id).innerHTML = http.responseText;
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
   var data = '';
   var attachment_type = '';
   var e;

   e = document.getElementById('searchtype');
   if(e && e.value) { data = data + "searchtype=" + e.value; }

   e = document.getElementById('_search');
   if(e && e.value) { data = data + "&search=" + e.value; }

   e = document.getElementById('from');
   if(e && e.value) { data = data + "&from=" + e.value; }

   e = document.getElementById('to');
   if(e && e.value) { data = data + "&to=" + e.value; }

   e = document.getElementById('subject');
   if(e && e.value) { data = data + "&subject=" + e.value; }

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

   e = document.getElementById('direction');
   if(e && e.value) { data = data + "&direction=" + e.value; }

   e = document.getElementById('action');
   if(e && e.value) { data = data + "&action=" + e.value; }

   for(i=0; i<attachment_types.length; i++) {
      e = document.getElementById('attachment_type_' + attachment_types[i]);
      if(e && e.checked == 1) { attachment_type += '|' + attachment_types[i]; }
   }

   if(attachment_type) { data = data + "&attachment_type=" + attachment_type.substring(1,attachment_type.length); }

   e = document.getElementById('ref');
   if(e && e.value) { data = data + "&ref=" + e.value; }

   e = document.getElementById('sort');
   if(e && e.value) { data = data + "&sort=" + e.value; }

   e = document.getElementById('order');
   if(e && e.value) { data = data + "&order=" + e.value; }


   //alert("data: " + data);

   return data;
}


function fix_search_order(sort, order) {
   var a;

   a = document.getElementById('sort');
   a.value = sort;

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

   a = document.getElementById('from'); a.value = '';
   a = document.getElementById('to'); a.value = '';
   a = document.getElementById('subject'); a.value = '';

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
   var idlist = '';

   var tag_keys = document.getElementById('tag_keys').value;
   var tag_value = document.getElementById('tag_value').value;

   if(tag_keys) {
      params = "idlist=" + tag_keys + "&tag=" + encodeURI(tag_value);

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


function restore_selected_emails(url) {
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

   if(idlist) {
      var http = getXMLHttp();

      if(http == null) { alert("Error creating XMLHttpRequest"); return; }

      document.getElementById('A1').innerHTML = '<div class="restore_spinner"><img src="/view/theme/default/images/spinner.gif" id="spinner_restore" alt="spinner" /></div>';


      http.open("POST", url, true);

      params = "idlist=" + idlist;

      http.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
      http.setRequestHeader("Content-length", params.length);
      http.setRequestHeader("Connection", "close");


      http.onreadystatechange = function() {
         if(http.readyState == 4) {
            if(http.status == 200) {
               document.getElementById('A1').innerHTML = http.responseText;
            }
            else {
               alert("Problem retrieving XML data:" + http.statusText);
            }
         }
      }

      http.send(params);
   }

   //document.getElementById('A1').innerHTML = '&nbsp;';
}


$(document).ready(function() {
   $.datepicker.setDefaults($.datepicker.regional[piler_ui_lang]);
   $("#date1").datepicker( { dateFormat: 'yy-mm-dd' } );
   $("#date2").datepicker( { dateFormat: 'yy-mm-dd' } );


   $(".messagelink").colorbox({iframe:true, width:"80%", height:"80%" });
   $(".savedsearch").colorbox({iframe:true, width:"400px", height:"300px", top:"50px", left:"50%"});

  });


