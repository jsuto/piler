
var Piler = 
{
    /* 
     * which search type is active, it's set by clicking on the 'Search' button
     */
    search:'',

    // legacy variable(s)
    expsrc: 0,
    health_refresh: <?php print HEALTH_REFRESH; ?>,
    piler_ui_lang: '<?php LANG == 'en' ? print 'en-GB' : print LANG; ?>',
    prev_message_id: 0,
    pos: -1,
    current_message_id: 0,
    folders: '',
    extra_folders: '',


    /*
     * variables used at search listing
     */
    Shared: {
        page:0,
        sort:1,
        order:'date',
        type: 'search'
    },

    /*
     * search data
     */
    Searches: {},

    /*
     * message meta ids
     */
    Messages:[],

    /*
     * System logger
     */
    log:function( )
    {
        if ( window.console  )
        {
            var 
            a = arguments,
            b = +new Date,
            c = window.console;
            
            if ( !a.length )
                c.clear();
            else if ( a.length > 1 )
                c.log(b, a[0], [].slice.call(arguments, 1));
            // c.log(b, a[0], JSON.stringify([].slice.call(a, 1)));
            else
                c.log(b, a[0]);                
        }
    },

    /**
     * Returns the javascript event source.
     * 
     * @param {Object}   a  Javascript event
     * @param {Logical} [b] If exist the event propagation NOT! stoped
     *
     * @return {Object<jQuery>} Javascript event source
     **/
    getSource:function( a, b )
    {
        Piler.log("[getSource]", a, b ); 

        if ( !b )
        {
            try {
                if ( a.stopPropagation )
                    a.stopPropagation();
                else
                    a.cancelBubble = !0;
            }
            catch ( e ) 
            {
                Piler.log("[getSource]", e ); 
            }
        }

        return $( a.target ? a.target : a.srcElement );
    },


    /*
     * Change the list order.
     * 
     *    HTML: <a class="VALAMI" onclick="Piler.changeOrder(this)" xid="date" xorder="0"></a>
     *     CSS: .VALAMI {
     *              background: url("/view/theme/default/images/arrowup.gif") no-repeat scroll center center transparent;
     *              cursor: pointer;
     *              float: left;
     *              height: 10px;
     *              width: 10px;
     *           }
     * @param {Object} a  Javascript event
     *
     */
    changeOrder:function( a )
    {
        a = $( a );// a == DOM element
        // a = Piler.getSource( a );// a == Javascript event

        Piler.Shared.sort = a.attr('xid');// {String} (date|from|subject|size)
        Piler.Shared.order = a.attr('xorder');// {Number} (0|1) -> (ASC|DESC)

        Piler.log("[changeOrder]", Piler.Shared.sort, Piler.Shared.order);

        Piler.load_search_results( );
    },


    /*
     * load search results to div
     *
     */

    load_search_results:function( ) 
    {
        var url;

        Piler.Shared.type == 'search' ? url = '/search-helper.php' : url = '/audit-helper.php';

        Piler.log("[load_search_results]", url); 

        Piler.spinner('start');

        jQuery.ajax( url, {
            data: $.extend(!0, {}, Piler.Shared, Piler.Searches[Piler.search]),
            type: "POST"
        })
        .done( function( a )// data, textStatus, jqXHR
        {

            if(a.indexOf('<?php print PILER_LOGIN_HELPER_PLACEHOLDER; ?>') > 0) {
               document.location.href = '<?php print SITE_URL; ?>';
               return true;
            }

            $('#mailcontframe').html(a);
            Piler.fill_current_messages_array();
            Piler.spinner('stop');
            $('#resultsheader').show();

<?php if(ENABLE_TABLE_RESIZE == 1) { ?>

            $("table").resizableColumns({
               store: store
            });

<?php } ?>

        })
        .fail(function( a, b )// jqXHR, textStatus, errorThrown
        {
            alert("Problem retrieving XML data:" + b)
        });
    },


    /*
     * show/hide spinner
     */

    spinner:function(cmd)
    {
        Piler.log("[spinner]", cmd);

        if(cmd == 'start') {
           $('#sspinner').show();
           $('#messagelistcontainer').hide();
        }

        if(cmd == 'stop') {
           $('#sspinner').hide();
           $('#messagelistcontainer').show();
        }
    },


    /*
     * save current search criteria
     */

    saved_search_terms:function(msg)
    {
        Piler.log("[saved_search_terms]");

        jQuery.ajax( '/index.php?route=search/save', {
            data: $.extend(!0, { }, Piler.Shared, Piler.Searches[Piler.search], {save: '1'} ),
            type: "POST"
        })
        .done( function( a )
        {
        })
        .fail(function( a, b )
        {
            alert("Problem retrieving XML data:" + b)
        });

        Piler.show_message('messagebox1', msg, 0.85);
    },


    /*
     * load saved search criteria list by ajax
     */

    load_saved_search_terms:function()
    {
        Piler.log("[load_saved_search_terms]");

        jQuery.ajax( '/index.php?route=search/load', {
        })
        .done( function( a )
        {
            $('#mailcontframe').html( a );
        })
        .fail(function( a, b )
        {
            alert("Problem retrieving XML data:" + b)
        });
    },


    remove_saved_search_term:function(ts)
    {
        Piler.log("[remove_saved_search_term]");

        jQuery.ajax('/index.php?route=search/remove&ts=' + ts, {})
        .done(function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.load_saved_search_terms();
    },


    /*
     * load the selected message by position to preview area
     */

    view_message_by_pos:function(pos)
    {
        Piler.log("[view_message_by_pos]", pos, Piler.Messages[pos]);

	if(pos == -1) return false;

        id = Piler.Messages[pos];

        Piler.pos = pos;

        if(Piler.prev_message_id > 0) { $('#e_' + Piler.prev_message_id).attr('class', 'resultrow'); }

        $('#e_' + id).attr('class', 'resultrow selected');

        Piler.prev_message_id = id;
        Piler.view_message(id);
    },


    view_message:function(id)
    {
        var search = $('#_search').val();

        Piler.log("[view_message]", id, search);

        jQuery.ajax('/message.php', {
           data: { id: id, search: search },
           type: "POST"
        })
        .done( function(a) {

           if(a.indexOf('<?php print PILER_LOGIN_HELPER_PLACEHOLDER; ?>') > 0) {
              document.location.href = '<?php print SITE_URL; ?>';
              return true;
           }

           $('#mailpreviewframe').html(a);
        })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    view_headers:function(id)
    {
        Piler.log("[view_headers]");
        Piler.load_url_to_preview_pane('/index.php?route=message/headers&id=' + id);
    },


    view_journal:function(id)
    {
         Piler.log("[view_journal]");
         Piler.load_url_to_preview_pane('/index.php?route=message/journal&id=' + id);
    },


    restore_message:function(id)
    {
        Piler.log("[restore_message]");
        Piler.load_url_to_preview_pane('/index.php?route=message/restore&id=' + id);
    },


    bulk_restore_messages:function(msg)
    {
        Piler.log("[bulk_restore_messages]");

        var idlist = Piler.get_selected_messages_list();

        if(!idlist) return;

        jQuery.ajax('/bulkrestore.php', {
           data: { download: '0', idlist: idlist },
           type: "POST"
        })
        .done( function( a ) {})
        .fail(function( a, b ) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('messagebox1', msg, 0.8);
    },


    /*
     * bulk toggle all the checkboxes for the result set
     */

    toggle_bulk_check:function(i)
    {
        Piler.log("[toggle_bulk_check]", Piler.Messages.length);

        $('#bulkcheck' + i).prop('checked') ? bulkcheck = 1 : bulkcheck = 0;

        Piler.log("[toggle_bulk_check], bulkcheck=", bulkcheck);

        for(i=0; i<Piler.Messages.length; i++) {
           bulkcheck == 1 ? $("#r_" + Piler.Messages[i]).prop('checked', true) : $("#r_" + Piler.Messages[i]).prop('checked', false);
        }

        if(bulkcheck == 1) {
           $("#bulkcheck").prop('checked', true);
           $("#bulkcheck2").prop('checked', true);
        } else {
           $("#bulkcheck").prop('checked', false);
           $("#bulkcheck2").prop('checked', false);
        }
    },


    add_note_to_message:function(id, msg)
    {
        Piler.log("[add_note_to_message]", id, msg);

        jQuery.ajax('index.php?route=message/note', {
           data: { id: id, note: encodeURI($('#note').val()) },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('messagebox1', msg, 0.85);
    },


    tag_search_results:function(msg)
    {
        Piler.log("[tag_search_results]", msg);

        var idlist = Piler.get_selected_messages_list();

	Piler.log("[tag_search_results, idlist]", idlist);

        if(!idlist) return false;

        jQuery.ajax('index.php?route=search/tag', {
           data: { tag: encodeURI($('#tag_value').val()), idlist: idlist },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('messagebox1', msg, 0.85);
    },


    /*
     * load the given url to the preview pane
     */

    load_url_to_preview_pane:function(url)
    {
        Piler.log("[load_url_to_preview_pane]", url);

        jQuery.ajax(url, { cache: true })
        .done( function(a) { $('#mailpreviewframe').html(a); })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    /*
     * return a comma separated list of selected messages
     */

    get_selected_messages_list:function()
    {
        Piler.log("[get_selected_messages_list]");

        var idlist = '';

        for(i=0; i<Piler.Messages.length; i++) {
           if($('#r_' + Piler.Messages[i]).prop('checked')) {
              if(idlist) idlist += ",";
              idlist += Piler.Messages[i];
              //Piler.log("[selected message, id:]", Piler.Messages[i]);
           }
        }

        Piler.log("[get_selected_messages_list], result:", idlist);

        return idlist;
    },


    /*
     * fill Messages array with search results
     */
 
    fill_current_messages_array:function() 
    {
        Piler.log("[fill_current_messages_array]" ); 
        
        var z = $('#results tbody').children(), y = z.length, x;
        var u = [];

        Piler.log("[fill_current_messages_array] y", y ); 

        for (i=0; i<y; i++)
        {
            x = z[i];

            if ( x.nodeName == "TR" && x.id.charAt( 0 ) == 'e' && x.id.charAt( 1 ) == '_' )
            {
                Piler.log("[fill_current_messages_array], pos/id", i, x.id.substring(2, 1000));

                u[i] = x.id.substring(2, 1000);
            }
        }

        Piler.Messages = u;
        Piler.pos = -1;
        Piler.prev_message_id = 0;
        Piler.current_message_id = 0;
    },   


    show_next_message:function()
    {
        if(Piler.pos < Piler.Messages.length-1) { Piler.pos++; }

        Piler.log("[show_next_message]", Piler.pos);

        Piler.view_message_by_pos(Piler.pos);
    },


    show_prev_message:function()
    {
        if(Piler.pos > 0) { Piler.pos--; }

        Piler.log("[show_prev_message]", Piler.pos);

        Piler.view_message_by_pos(Piler.pos);
    },


    /*
     * load the search results for a saved query
     * TODO: fix searchtype, it can be even 'complex', too
     */

    load_search_results_for_saved_query:function(terms)
    {
        Piler.log("[load_search_results_for_saved_query]", terms);

        terms = decodeURIComponent(terms);

        var pairs = terms.split('&');
        $.each(pairs, function(i, v){
           var pair = v.split('=');
           if(pair[0] == 'search') {
              var search = decodeURIComponent(pair[1]);
              $("input#_search").val(search.replace(/\+/g, " "));
           }
        });

        Piler.expsrc++;
        $('#_search').css('color', 'black');

        Piler.expert();
    },


    /*
     * expert search
     * 
     *    HTML: <button onclick="Piler.simple(this)">Search</button>
     *    <button onclick="script:var a=document.getElementById('ref'); if(a) a.value=''; a = document.getElementById('prefix'); if(a) a.value=''; 
     *    load_search_results('http://demo.mailpiler.org/search-helper.php', assemble_search_term(count), 0);" 
     *    style="margin-left: 10px; margin-right: 0px; height: 20px; width: 70px;" class="active" id="button_search">Search</button>    
     **/
    expert:function( )// a )
    {
        Piler.log("[expert]")//, a ); 

        // a = $( a );// a == DOM element
        // a = Piler.getSource( a );// a == Javascript event

        $('#prefix').val('');

        Piler.search = 'Expert';
        
        Piler.Shared.page = 0;
        Piler.Shared.type = 'search';

        Piler.assemble_folder_restrictions();

        Piler.Searches.Expert = {
            search: $.trim($('input#_search').val()),
            searchtype: 'expert',
            ref: $.trim($('#ref').val()),
            folders: Piler.folders,
            extra_folders: Piler.extra_folders
        }

        $('#ref').val('');
 
        Piler.load_search_results();
    },   


    /**
     * complex search
     * 
     *    HTML: <button id="simple" class="active" onclick="Piler.complex(this)">Search</button>
     *     CSS: #expert { margin-left: 10px; margin-right: 0px; height: 20px; width: 70px; }
     *     
     **/
    complex:function( )// a )
    {
        Piler.log("[complex]")//, a ); 

        // a = $( a );// a == DOM element
        // a = Piler.getSource( a );// a == Javascript event

        var z = $('div#searchpopup1');

        Piler.search = 'Complex';

        Piler.Shared.page = 0;
        Piler.Shared.type = 'search';

        Piler.assemble_folder_restrictions();

        var attachments_type = '';

        if($('input#xhas_attachment_any', z)[0].checked) { attachments_type += ',any'; }
        if($('input#xhas_attachment_doc', z)[0].checked) { attachments_type += ',word'; }
        if($('input#xhas_attachment_xls', z)[0].checked) { attachments_type += ',excel'; }
        if($('input#xhas_attachment_pdf', z)[0].checked) { attachments_type += ',pdf'; }
        if($('input#xhas_attachment_image', z)[0].checked) { attachments_type += ',image'; }

        if(attachments_type) { attachments_type = attachments_type.substring(1, attachments_type.length); }

        Piler.Searches.Complex = {
            from : $.trim($('input#xfrom', z).val()),
            to : $.trim($('input#xto', z).val()),
            subject : $.trim($('input#xsubject', z).val()),
            body : $.trim($('input#xbody', z).val()),
            tag : $.trim($('input#xtag', z).val()),
            note : $.trim($('input#xnote', z).val()),
            attachment_type : attachments_type,
            date1 : $.trim($('input#date1', z).val()),
            date2 : $.trim($('input#date2', z).val()),
            searchtype : 'simple',
            folders: Piler.folders,
            extra_folders: Piler.extra_folders
        }

        Piler.load_search_results();

        $('#searchpopup1').hide();
    },


    /*
     * paging function
     * 
     *    HTML: <a onclick="Piler.navigation(${PHP_PAGE})" class="navlink">${next page}</a>
     *    
     */
    navigation:function( a )
    {
        Piler.log("[navigation]")//, a ); 

        // a = $( a );// a == DOM element
        // a = Piler.getSource( a );// a == Javascript event
        
        Piler.Shared.page = a;
        
        Piler.load_search_results();
    },


    /*
     * reset search fields
     * 
     *    HTML: <input type="button" onclick="Piler.cancel()" value="Cancel">
     *     CSS: input.advsecondary[type="button"]{ height: 20px; width: 70px; }
     */
    cancel:function( )//a )
    {
        Piler.log("[cancel]")//, a ); 

        // a = $( a );// a == DOM element
        // a = Piler.getSource( a );// a == Javascript event

        $('#_search').val(''); 
        $('#ref').val( '' );

        Piler.Searches.Expert = {};

        $('input#xfrom').val('');
        $('input#xto').val('');
        $('input#xsubject').val('');
        $('input#xbody').val('');
        $('input#xtag').val('');
        $('input#xnote').val('');
        $('input#xhas_attachment_doc')[0].checked = 0;
        $('input#xhas_attachment_xls')[0].checked = 0;
        $('input#xhas_attachment_pdf')[0].checked = 0;
        $('input#xhas_attachment_image')[0].checked = 0;
        $('input#xhas_attachment_any')[0].checked = 0;
        $('input#date1').val('');
        $('input#date2').val('');

        Piler.Searches.Complex = {};
    },


    /*
     * when clicked on the "any" attachment type, clear the other types
     */

    clear_attachment_others: function()
    {
        $('input#xhas_attachment_doc')[0].checked = 0;
        $('input#xhas_attachment_xls')[0].checked = 0;
        $('input#xhas_attachment_pdf')[0].checked = 0;
        $('input#xhas_attachment_image')[0].checked = 0;
    },


    /*
     * when clicked on any of the named attachment types, clear the "any" type
     */

    clear_attachment_any: function()
    {
        $('input#xhas_attachment_any')[0].checked = 0;       
    },


    /*
     * show a temporary message to the user
     */

    show_message:function(id, msg, timeout)
    {
        msg = '<p>' + msg.replace("\n", "<br />") + '</p>'; 

        Piler.log("[show_message]", id, msg);

        $('#'+id).html(msg);
        $('#'+id).show();
        setTimeout(function() { $('#'+id).hide(); }, timeout*1000);
    },


    load_health:function()
    {
        Piler.log("[load_health]");

        document.body.style.cursor = 'wait';

        jQuery.ajax('/index.php?route=health/worker', { })
        .done( function(a) {
           $('#A1').html(a);
           document.body.style.cursor = 'default';

           //setInterval('Piler.load_health()', Piler.health_refresh * 1000);
        })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    /*
     * show a hint message for an autocomplete field on user/group editing
     */

    toggle_hint:function(id, s, focus)
    {
        Piler.log("[toggle_hint]", id);

        if(focus == 1){
           if($('#' + id).val() == s) $('#' + id).val('');
        }
        else {
           if($('#' + id).val() == '') $('#' + id).val(s);
        }
    },


    /*
     * assemble a space separated list of the selected email addresses of the given message
     */

    restore_message_for_recipients:function(id, msgok, msgerr)
    {
        Piler.log("[restore_message_for_recipients]", id);

        var z = $('#restorebox').children(), y = z.length, x;
        var emails = '';

        for ( ; --y >= 0 ; )
        {
            x = z[y];

            if (x.id.substring(0, 5) == "rcpt_" )
            {
                if(document.getElementById(x.id).checked == 1){
                   if(emails) emails += ' ';
                   emails += x.id.substring(5, 1000);
                }
            }
        }

        if(emails) {
           jQuery.ajax('index.php?route=message/restore', {
              data: { rcpt: encodeURI(emails), id: id },
              type: "POST"
           })
           .done( function(a) {})
           .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

           Piler.show_message('messagebox1', msgok, 0.85);
           $('#restorebox').hide();
        }
        else {
           Piler.show_message('messagebox1', msgerr, 0.85);
        }

    },


    download_messages_real:function(idlist)
    {
        Piler.log("[download_messages_real]", idlist);

        if(idlist) {
           var form = document.createElement("form");

           form.setAttribute("method", "post");
           form.setAttribute("action", '/bulkrestore.php');
           form.setAttribute("name", "download");

           var hiddenField = document.createElement("input");

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

           document.body.removeChild(form);
       }
    },


    download_messages:function()
    {
        var idlist = Piler.get_selected_messages_list();
        Piler.download_messages_real(idlist);
    },


    download_all_search_hits:function()
    {
        var url;

        url = '/index.php?route=message/dl';

        jQuery.ajax( url, {
            type: "POST"
        })
        .done( function( a )// data, textStatus, jqXHR
        {
            Piler.download_messages_real(a);
        })
        .fail(function( a, b )// jqXHR, textStatus, errorThrown
        {
            alert("Problem retrieving XML data:" + b)
        });

    },


    auditexpert:function()
    {
        Piler.log("[auditexpert]");

        Piler.search = 'Expert';

        Piler.Shared.page = 0;
        Piler.Shared.type = 'audit';

        Piler.Searches.Expert = {
            search : $('input#_search').val().trim(),
            searchtype : 'expert'
        }

        Piler.load_search_results();
    },


    /*
     * add shortcuts on the search page
     */

    add_shortcuts:function()
    {
        Piler.log("[add_shortcuts]");

        $(document).keypress(function(e){
           if(e.which == 13){
              $("#button_search").click();
           }

           // 37: left, 38: up, 39: right, 40: down

           if(e.keyCode == 38){
              Piler.show_prev_message();
           }

           if(e.keyCode == 40){
              Piler.show_next_message();
           }

        });

    },


    reload_piler:function()
    {
        Piler.log("[reload_piler]");

        jQuery.ajax('index.php?route=policy/apply', { cache: true })
        .done( function(a) {
           $('#applyChangesOutput').html(a);
        })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    assemble_folder_restrictions: function()
    {
       Piler.log("[assemble_folder_restrictions]");

       Piler.folders = '';
       Piler.extra_folders = '';

       a = document.getElementById('folders');
       if(!a) { return false; }

       childNodeArray = a.getElementsByTagName('*');

       if(childNodeArray) {
          for(i=0; i<childNodeArray.length; i++) {
             b = childNodeArray[i];
             if(b.name && b.name.substring(0, 7) == 'folder_' && b.checked) {
                Piler.folders = Piler.folders + " " + b.name.substring(7);
             }

             if(b.name && b.name.substring(0, 13) == 'extra_folder_' && b.checked) {
                Piler.extra_folders = Piler.extra_folders + " " + b.name.substring(13);
             }

          }
       }

       if(Piler.folders) {
          Piler.folders = Piler.folders.substring(1);
       }

       if(Piler.extra_folders) {
          Piler.extra_folders = Piler.extra_folders.substring(1);
       }

       Piler.log("[folder/extra_folders]", Piler.folders, Piler.extra_folders);
    },


    copy_message_to_folder: function(folder_id, id, msg)
    {
        Piler.log("[copy_message_to_folder]", folder_id, id);

        if(id <= 0) { return 0; }

        var folder_copy_url = '<?php print SITE_URL; ?>/index.php?route=folder/copy'

        jQuery.ajax('index.php?route=folder/copy', {
           data: { folder_id: folder_id, id: id },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('messagebox1', msg, 0.85);
 
        Piler.current_message_id = 0;
    },


    open_folder: function(id)
    {
       $('#fldr_' + id).show();
       $('#fldr_collapse_' + id).show();
       $('#fldr_open_' + id).hide();
    },


    close_folder: function(id)
    {
       $('#fldr_' + id).hide();
       $('#fldr_collapse_' + id).hide();
       $('#fldr_open_' + id).show();
    },


    stop_propagation: function(event) {
       Piler.log("[stop_propagation]");
       try {
          event.stopPropagation();
       }
       catch ( e ) {
          Piler.log("[stop_propagation]", e );
       }
    },


    test_ldap_connection:function()
    {
       Piler.log("[test_ldap_connection]");
 
       jQuery.ajax('index.php?route=ldap/test', {
           data: {
              description: $('#description').val(),
              ldap_host: $('#ldap_host').val(),
              ldap_base_dn: $('#ldap_base_dn').val(),
              ldap_bind_dn: $('#ldap_bind_dn').val(),
              ldap_bind_pw: $('#ldap_bind_pw').val()
           },
           type: "POST"
       })
       .done( function(a) {
          $('#LDAPTEST').html(a);
       })
       .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    test_pop3_connection:function()
    {
       Piler.log("[test_pop3_connection]");

       jQuery.ajax('index.php?route=import/test', {
           data: {
              type: $('#type').val(),
              server: $('#server').val(),
              username: $('#username').val(),
              password: $('#password').val()
           },
           type: "POST"
       })
       .done( function(a) {
          $('#LDAPTEST').html(a);
       })
       .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    clear_ldap_test: function()
    {
        $('#LDAPTEST').html('');
    },


    reload_page: function()
    {
        location.reload(true);
    },


    go_to_default_page: function()
    {
       document.location.href = 'search.php';
    },


    change_box_colour: function(srcid, dstid)
    {
       var colour = $('#' + srcid).val();
       $('#' + dstid).css('background', colour);
    }


}



var split = new rcube_webmail();

$.datepicker.setDefaults($.datepicker.regional[Piler.piler_ui_lang]);



  $(function() {

    $("#s_piler_email").autocomplete({
        source: '/index.php?route=group/email&',
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';

                   if($('#email').val() == '') prefix = '';

                   $('#email').val($('#email').val() + prefix + ui.item.value);
                }

                ui.item.value = '';
        }
    });

    $("#s_piler_domain").autocomplete({
        source: '/index.php?route=domain/domains&',
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';

                   if($('#domains').val() == '') prefix = '';

                   $('#domains').val($('#domains').val() + prefix + ui.item.value);

                }

                ui.item.value = '';
        }

    });


    $("#s_piler_group").autocomplete({
        source: '/index.php?route=group/group&',
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';

                   if($('#group').val() == '') prefix = '';

                   $('#group').val($('#group').val() + prefix + ui.item.value);
                }

                ui.item.value = '';
        }
    });

    $("#s_piler_folder").autocomplete({
        source: '/index.php?route=folder/folder&',
        minLength: 2,
        select: function( event, ui ) {
                if(ui.item){
                   var prefix = '\n';

                   if($('#folder').val() == '') prefix = '';

                   $('#folder').val($('#folder').val() + prefix + ui.item.value);
                }

                ui.item.value = '';
        }
    });


    $("ul.dropdown li").hover(function(){
    
        $(this).addClass("hover");
        $('ul:first',this).css('visibility', 'visible');
    
    }, function(){
    
        $(this).removeClass("hover");
        $('ul:first',this).css('visibility', 'hidden');
    
    });


    $("#date1").datepicker( {dateFormat: '<?php print JQUERY_DATE_FORMAT; ?>' });
    $("#date2").datepicker( {dateFormat: '<?php print JQUERY_DATE_FORMAT; ?>' });


  });

// modal additions
    
$(document).on("click", ".confirm-delete", function (e) {
     e.preventDefault();


     var id = $(this).data('id'),
         name = $(this).data('name'),
         url = $(".modal-footer #id").attr("href");
     
     //set id
     url = UpdateQueryString('id',id,url);
     //set name
     url = UpdateQueryString('name',encodeURIComponent(name),url);
     //set confirmation
     url = UpdateQueryString('confirmed',1,url);
     //set href
     $(".modal-footer #id").attr("href",url);
     //set display text
     $(".modal-body #name").html( name );
     //finally, display the confirm modal box
     $('#deleteconfirm-modal').modal('show');
});

function UpdateQueryString(key, value, url) { // from http://stackoverflow.com/questions/5999118/add-or-update-query-string-parameter/11654596#11654596
    if (!url) url = window.location.href;
    var re = new RegExp("([?|&])" + key + "=.*?(&|#|$)(.*)", "gi");

    if (re.test(url)) {
        if (typeof value !== 'undefined' && value !== null)
            return url.replace(re, '$1' + key + "=" + value + '$2$3');
        else {
            return url.replace(re, '$1$3').replace(/(&|\?)$/, '');
        }
    }
    else {
        if (typeof value !== 'undefined' && value !== null) {
            var separator = url.indexOf('?') !== -1 ? '&' : '?',
                hash = url.split('#');
            url = hash[0] + separator + key + '=' + value;
            if (hash[1]) url += '#' + hash[1];
            return url;
        }
        else
            return url;
    }
}


