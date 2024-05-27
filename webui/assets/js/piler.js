let Piler =
{
    /*
     * which search type is active, it's set by clicking on the 'Search' button
     */
    search:'',

    base_url: location.origin + '/',

    // legacy variable(s)
    expsrc: 0,
    prev_message_id: 0,
    pos: -1,
    current_message_id: 0,
    folders: '',
    extra_folders: '',

    remove_message_id: 0,

    login_text: 'PILER_COMMENT_FOR_PROPER_LOGIN_SO_THIS_CAN_BE_ANYTHING_JUST_BE_IT_SOMETHING_LIKE_A_UNIQUE_VALUE',

    refresh_interval: 300,

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

    /*
     * Change the list order.
     */
    changeOrder:function(a)
    {
        a = $(a);

        Piler.Shared.sort = a.attr('xid');
        Piler.Shared.order = a.attr('xorder');

        Piler.log("[changeOrder]", Piler.Shared.sort, Piler.Shared.order);

        Piler.load_search_results( );
    },


    /*
     * load search results to div
     *
     */

    load_search_results:function( )
    {
        let url;

        Piler.poor_mans_keepalive_for_dummy_browsers();

        Piler.Shared.type == 'search' ? url = Piler.base_url + 'search-helper.php' : url = Piler.base_url + 'audit-helper.php';

        Piler.log("[load_search_results]", url);

        Piler.spinner('start');

        jQuery.ajax(url, {
            data: $.extend(!0, {}, Piler.Shared, Piler.Searches[Piler.search]),
            type: "POST"
        })
        .done( function(a)
        {
            if(a.indexOf(Piler.login_text) > 0) {
               document.location.href = Piler.base_url;
               return true;
            }

            //$('#mailcontframe').html(a);
            $('#qqq').html(a);
            Piler.fill_current_messages_array();
            Piler.spinner('stop');
            //$('#resultsheader').show();
        })
        .fail(function(a, b)
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
          document.getElementById("spinner").classList.remove("hidden");
        }

        if(cmd == 'stop') {
          document.getElementById("spinner").classList.add("hidden");
        }
    },


    /*
     * save current search criteria
     */

    saved_search_terms:function(msg)
    {
        Piler.log("[saved_search_terms]");

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=search/save', {
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

        Piler.show_message('INFO', msg);
    },


    /*
     * load saved search criteria list by ajax
     */

    load_saved_search_terms:function()
    {
        Piler.log("[load_saved_search_terms]");

        jQuery.ajax(Piler.base_url + 'index.php?route=search/load', {
        })
        .done( function( a )
        {
            $('#qqq').html( a );
        })
        .fail(function( a, b )
        {
            alert("Problem retrieving XML data:" + b)
        });
    },


    remove_saved_search_term:function(ts)
    {
        Piler.log("[remove_saved_search_term]");

        jQuery.ajax(Piler.base_url + 'index.php?route=search/remove&ts=' + ts, {})
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

        $('#e_' + id).attr('class', 'resultrow selected table-info');

        Piler.prev_message_id = id;
        Piler.view_message(id);

        $('#preview').scrollTop(0);
    },


    view_message:function(id)
    {
        let search = $('#_search').val();

        Piler.poor_mans_keepalive_for_dummy_browsers();

        Piler.log("[view_message]", id, search);

        jQuery.ajax(Piler.base_url + 'message.php', {
           data: { id: id, search: search },
           type: "POST"
        })
        .done( function(a) {
           if(a.indexOf(Piler.login_text) > 0) {
              document.location.href = Piler.base_url;
              return true;
           }

           $('#preview').html(a);
        })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    view_headers:function(id)
    {
        Piler.log("[view_headers]");
        Piler.load_url_to_preview_pane(Piler.base_url + 'index.php?route=message/headers&id=' + id);
    },


    view_journal:function(id)
    {
        Piler.log("[view_journal]");
        Piler.load_url_to_preview_pane(Piler.base_url + 'index.php?route=message/journal&id=' + id);
    },


    not_spam:function(id)
    {
        Piler.log("[not_spam]", id);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=message/notspam', {
           data: { id: id },
           type: "POST"
        })
        .done( function( a ) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', "OK");
    },


    mark_as_private:function(id, val)
    {
       Piler.log("[mark_as_private]", id, val);

       Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=message/private', {
           data: { id: id, val: val },
           type: "POST"
        })
        .done( function( a ) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', "OK");
    },


    restore_message:function(id)
    {
        Piler.log("[restore_message]");

        jQuery.ajax(Piler.base_url + 'index.php?route=message/restore&id=' + id, {})
        .done(function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', "OK");
    },


    show_advanced_search_modal:function()
    {
        Piler.log("[show_advanced_search_modal]");
        $('#advancedsearch-modal').modal('show');
    },


    bulk_remove_messages:function(reason, text_successfully_removed, text_no_selected_message)
    {
        Piler.log("[bulk_remove_messages]");

        Piler.poor_mans_keepalive_for_dummy_browsers();

        let idlist = Piler.get_selected_messages_list();

        if(!idlist) {
           Piler.show_message('ERROR', text_no_selected_message);
           return;
        }

        jQuery.ajax(Piler.base_url + 'bulkremove.php', {
           data: { idlist: idlist, reason: reason },
           type: "POST"
        })
        .done( function( a ) {})
        .fail(function( a, b ) { alert("Problem retrieving XML data:" + b) });

        Piler.modal('deleteModal');

        Piler.show_message('OK', text_successfully_removed);
    },


    approve_message_removal:function()
    {
        Piler.log("[approve_message_removal]", Piler.remove_message_id);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=message/remove', {
        data: { id: Piler.remove_message_id, confirmed: 1 },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.modal('removeApproveModal');

        Piler.show_message('OK', 'Approved removal of message');
    },


    reject_message_removal:function()
    {
        Piler.log("[reject_message_removal]", Piler.remove_message_id);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        let reason2 = $('#reason2').val();

        if(reason2) {
          jQuery.ajax(Piler.base_url + 'rejectremove.php', {
          data: { id: Piler.remove_message_id, confirmed: 1, reason2: reason2 },
             type: "POST"
          })
          .done( function(a) {})
          .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

          Piler.modal('removeRejectModal');

          Piler.show_message('OK', 'Removal of message is rejected');
        } else {
          Piler.show_message('ERROR', 'Please provide reason for rejection');
        }
    },


    bulk_restore_messages:function(msg, email)
    {
        Piler.log("[bulk_restore_messages]", email);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        let idlist = Piler.get_selected_messages_list();

        if(!idlist) return;

        jQuery.ajax(Piler.base_url + 'bulkrestore.php', {
           data: { download: '0', idlist: idlist, email: email },
           type: "POST"
        })
        .done( function( a ) {})
        .fail(function( a, b ) { alert("Problem retrieving XML data:" + b) });

        Piler.modal('bulkRestoreModal');

        Piler.show_message('INFO', msg);
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

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=message/note', {
           data: { id: id, note: encodeURI($('#note').val()) },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', msg);
    },


    update_message_folder:function(id, msg)
    {
        Piler.log("[update_message_folder]", id, msg);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=message/folder', {
           data: { id: id, folder_id: $('#folder_id').val() },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', msg);
    },


    tag_search_results:function(msg)
    {
        Piler.log("[tag_search_results]", msg);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        let idlist = Piler.get_selected_messages_list();

        Piler.log("[tag_search_results, idlist]", idlist);

        if(!idlist) return false;

        jQuery.ajax(Piler.base_url + 'index.php?route=search/tag', {
           data: { tag: encodeURI($('#tag_value').val()), idlist: idlist },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', msg);
    },


    /*
     * load the given url to the preview pane
     */

    load_url_to_preview_pane:function(url)
    {
        Piler.log("[load_url_to_preview_pane]", url);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(url, { cache: true })
        .done( function(a) { $('#preview').html(a); })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });
    },


    /*
     * return a comma separated list of selected messages
     */

    get_selected_messages_list:function()
    {
        Piler.log("[get_selected_messages_list]");

        let idlist = '';

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


    get_messages_list:function()
    {
        let idlist = '';

        for(i=0; i<Piler.Messages.length; i++) {
           if(idlist) idlist += ",";
           idlist += Piler.Messages[i];
        }

        return idlist;
    },


    /*
     * fill Messages array with search results
     */

    fill_current_messages_array:function()
    {
        Piler.log("[fill_current_messages_array]");

        let z = $('#results tbody').children(), y = z.length, x;
        let u = [];

        Piler.log("[fill_current_messages_array] y", y );

        if (!y) { Piler.show_message('none found'); }

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

        let pairs = terms.split('&');
        $.each(pairs, function(i, v){
           let pair = v.split('=');
           if(pair[0] == 'search') {
              let search = decodeURIComponent(pair[1]);
              $("input#_search").val(search.replace(/\+/g, " "));
           }
        });

        Piler.expsrc++;
        $('#_search').css('color', 'black');

        Piler.expert();
    },


    /*
     * expert search
     **/
    expert:function()
    {
        Piler.log("[expert]");

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
     **/
    complex:function( )
    {
        Piler.log("[complex]");

        let z = $('div#advancedSearchModal');

        Piler.search = 'Complex';

        Piler.Shared.page = 0;
        Piler.Shared.type = 'search';

        Piler.assemble_folder_restrictions();

        let attachments_type = '';

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

        $('input#_search').val('');

        Piler.modal('advancedSearchModal');

        Piler.load_search_results();
    },


    /*
     * paging function
     */
    navigation:function( a )
    {
        Piler.log("[navigation]");

        Piler.Shared.page = a;

        Piler.load_search_results();
    },


    /*
     * reset search fields
     */
    cancel:function( )//a )
    {
        Piler.log("[cancel]")//, a );

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


    load_health:function()
    {
        Piler.log("[load_health]");

        document.body.style.cursor = 'wait';

        jQuery.ajax(Piler.base_url + 'index.php?route=health/worker', { })
        .done( function(a) {
           $('#A1').html(a);
           document.body.style.cursor = 'default';
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


    restore_message_for_recipient:function(id, rcpt, msgok, msgerr)
    {
        Piler.log("[restore_message_for_recipient]", id, rcpt);

        Piler.poor_mans_keepalive_for_dummy_browsers();

        if(rcpt) {
           jQuery.ajax(Piler.base_url + 'index.php?route=message/restore', {
              data: { rcpt: encodeURI(rcpt), id: id },
              type: "POST"
           })
           .done( function(a) {})
           .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

           Piler.show_message('OK', msgok);
           Piler.modal('restoreModal');
        }
        else {
           Piler.show_message('ERROR', msgerr);
        }

    },


    download_messages_real:function(idlist, url)
    {
        Piler.log("[download_messages_real]", idlist);

        if(idlist) {
           let form = document.createElement("form");

           form.setAttribute("method", "post");
           form.setAttribute("action", url);
           form.setAttribute("name", "download");

           let hiddenField = document.createElement("input");

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
        let idlist = Piler.get_selected_messages_list();
        Piler.download_messages_real(idlist, Piler.base_url + 'bulkrestore.php');
    },


    download_all_search_hits:function()
    {
        Piler.log("[download_all_search_hits]");

        jQuery.ajax(Piler.base_url + 'index.php?route=message/dl', {
            type: "POST"
        })
        .done( function( a )
        {
            Piler.download_messages_real(a, Piler.base_url + 'bulkrestore.php');
        })
        .fail(function( a, b )
        {
            alert("Problem retrieving XML data:" + b)
        });

    },


    download_selected_as_pdf:function()
    {
        Piler.log("[download_selected_as_pdf]");

        let idlist = Piler.get_selected_messages_list();
        if(idlist) {
           Piler.download_messages_real(idlist, Piler.base_url + 'bulkpdf.php');
        }
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

        $("#button_search").click();

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

        jQuery.ajax(Piler.base_url + 'index.php?route=policy/apply', { cache: true })
        .done( function(a) {
           Piler.show_message(a);
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

        Piler.poor_mans_keepalive_for_dummy_browsers();

        jQuery.ajax(Piler.base_url + 'index.php?route=folder/copy', {
           data: { folder_id: folder_id, id: id },
           type: "POST"
        })
        .done( function(a) {})
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

        Piler.show_message('INFO', msg);

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

       jQuery.ajax(Piler.base_url + 'index.php?route=ldap/test', {
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
       $('#LDAPTEST').html('....');

       jQuery.ajax(Piler.base_url + 'index.php?route=import/test', {
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
       let colour = $('#' + srcid).val();
       $('#' + dstid).css('background', colour);
    },


    new_qr: function()
    {

        Piler.log("[new_qr]");

        document.body.style.cursor = 'wait';

        jQuery.ajax(Piler.base_url + 'qr.php?refresh=1', { cache: false })
        .done( function(a) {
           $('#QR').html(a);
           document.body.style.cursor = 'default';
        })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

    },


    toggle_ga: function()
    {
        let ga = 0;

        if(document.getElementById('ga_enabled').checked == 1){ ga = 1; }

        Piler.log("[toggle GA]", ga);

        jQuery.ajax(Piler.base_url + 'qr.php?toggle=' + ga, { cache: false })
        .done( function(a) {
        })
        .fail(function(a, b) { alert("Problem retrieving XML data:" + b) });

    },


    fix_ldap_display: function()
    {
       if($('#ldap_type').val() == 'generic_ldap') {
          $('#ldap_mail_attr_id').show();
          $('#ldap_account_objectclass_id').show();
          $('#ldap_distributionlist_attr_id').show();
          $('#ldap_distributionlist_objectclass_id').show();
       }
       else {
          $('#ldap_mail_attr').val('');
          $('#ldap_account_objectclass').val('');
          $('#ldap_distributionlist_attr').val('');
          $('#ldap_distributionlist_objectclass').val('');

          $('#ldap_mail_attr_id').hide();
          $('#ldap_account_objectclass_id').hide();
          $('#ldap_distributionlist_attr_id').hide();
          $('#ldap_distributionlist_objectclass_id').hide();
       }
    },


    print_div: function(divID) {
       let divElements = document.getElementById(divID).innerHTML;
       let oldPage = document.body.innerHTML;

       document.body.innerHTML = "<html><head><title></title></head><body>" + divElements + "</body></html>";

       window.print();

       document.body.innerHTML = oldPage;
    },


    poor_mans_keepalive_for_dummy_browsers: function() { },

    modal: function(id, action='') {
      try {
        let modalId = document.getElementById(id)
        const modal = bootstrap.Modal.getOrCreateInstance(modalId)
        if(action == 'show') {
          modal.show()
        } else {
          modal.hide()
        }
      } catch(e) {
        Piler.log(e)
      }
    },


    show_message: function(title = '', msg = '', small = "Now") {
      Piler.log("[show_message]")

      let toastTitle = document.getElementById("toast-title")
      let toastSmall = document.getElementById("toast-small")
      let toastMsg = document.getElementById("toast-msg")

      toastTitle.innerHTML = title
      toastSmall.innerHTML = small
      toastMsg.innerHTML = msg

      const toastId = document.getElementById("ToastId")
      if(toastId) {
        const toastBootstrap = bootstrap.Toast.getOrCreateInstance(toastId)
        toastBootstrap.show()
      }
    }

}

$(document).ready(function() {
  const queryString = window.location.search;
  const urlParams = new URLSearchParams(queryString);
  const route = urlParams.get('route');

  switch (route) {
    case 'health/health':
      Piler.load_health();
      setInterval('Piler.load_health()', Piler.refresh_interval * 1000);
      break;

    case 'stat/online':
    case 'import/jobs':
      setInterval('Piler.reload_page()', Piler.refresh_interval * 1000);
      break;

    case 'audit/audit':
    case 'audit/removal':
      Piler.add_shortcuts();
      break;

    default:
      const s = window.location.href;
      if(/search.php#?$/.test(s)) {
        Piler.add_shortcuts();
        Piler.expert();
      }

      break;
  }

});
