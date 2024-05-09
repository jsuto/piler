<!DOCTYPE html>
<html lang="<?php print DEFAULT_LANG; ?>">
<head>
   <title><?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="<?php print DEFAULT_LANG; ?>" />
   <meta name="viewport" content="width=device-width, initial-scale=1.0">

   <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
   <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
   <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
   <meta name="rating" content="general" />
   <meta name="robots" content="all" />
   <meta http-equiv="x-ua-compatible" content="IE=edge">

    <link href="/view/theme/default/assets/css/metro-bootstrap.css" rel="stylesheet">

    <!-- HTML5 shim, for IE6-8 support of HTML5 elements -->
    <!-- original location: http://html5shim.googlecode.com/svn/trunk/html5.js -->
    <!--[if lt IE 9]>
      <script src="/view/theme/default/assets/js/html5.js"></script>
      <style>body{padding-top:70px;}</style>
    <![endif]-->

    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>

    <?php print JS_CODE; ?>
</head>

<body onload="Piler.add_shortcuts();<?php if(ENABLE_INSTANT_SEARCH){ ?>Piler.expert();<?php } ?>">

<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_forward_selected_emails_to; ?></h3>
  </div>
  <div class="modal-body">
    <input type="text" id="restore_address" name="restore_address" />
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="#" onclick="var addr =  $('#restore_address').val(); if(addr) { Piler.bulk_restore_messages('<?php print $text_restored; ?>', addr); }" class="btn btn-primary" data-dismiss="modal" aria-hidden="true">OK</a>
  </div>
</div>

<div id="advancedsearch-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_advanced_search; ?></h3>
  </div>
  <div class="modal-body">

     <input type="hidden" name="xsearchtype" id="xsearchtype" value="simple" />
     <input type="hidden" name="xsort" id="xsort" value="date" />
     <input type="hidden" name="xorder" id="xorder" value="0" />
     <input type="hidden" name="xref" id="xref" value="" />

    <table class="table">
      <tr>
        <td><?php print $text_from; ?></td>
        <td><input type="text" name="xfrom" id="xfrom" value="" /></td>
      </tr>

      <tr>
        <td><?php print $text_to; ?></td>
        <td><input type="text" name="xto" id="xto" value="<?php if(isset($to)) { print $to; } ?>" /></td>
      </tr>

      <tr>
        <td><?php print $text_subject; ?></td>
        <td><input type="text" name="xsubject" id="xsubject" value="<?php if(isset($subject)) { print $subject; } ?>" /></td>
      </tr>

      <tr>
        <td><?php print $text_body; ?></td>
        <td><input type="text" name="xbody" id="xbody" value="<?php if(isset($body)) { print $body; } ?>" /></td>
      </tr>

      <tr>
        <td><?php print $text_tags; ?></td>
        <td><input type="text" name="xtag" id="xtag" value="<?php if(isset($tag)) { print $tag; } ?>" /></td>
      </tr>

      <tr>
        <td><?php print $text_notes; ?></td>
        <td><input type="text" name="xnote" id="xnote" value="<?php if(isset($note)) { print $note; } ?>" /></td>
      </tr>

      <tr>
        <td><?php print $text_attachment; ?></td>
        <td>
          <img src="view/theme/default/assets/images/fileicons/doc.png" alt="Word" title="Word" />
          <input type="checkbox" class="checkbox popup" name="xhas_attachment_doc" id="xhas_attachment_doc" <?php if(isset($has_attachment_doc) && $has_attachment_doc == 1) { ?>checked="checked"<?php } ?> onclick="Piler.clear_attachment_any();" />

          <img src="view/theme/default/assets/images/fileicons/xls.png" alt="Excel" title="Excel" />
          <input type="checkbox" class="checkbox popup" name="xhas_attachment_xls" id="xhas_attachment_xls" <?php if(isset($has_attachment_xls) && $has_attachment_xls == 1) { ?>checked="checked"<?php } ?> onclick="Piler.clear_attachment_any();" />

          <img src="view/theme/default/assets/images/fileicons/pdf.png" alt="PDF" title="PDF" />
          <input type="checkbox" class="checkbox popup" name="xhas_attachment_pdf" id="xhas_attachment_pdf" <?php if(isset($has_attachment_pdf) && $has_attachment_pdf == 1) { ?>checked="checked"<?php } ?> onclick="Piler.clear_attachment_any();" />

          <img src="view/theme/default/assets/images/fileicons/image.png" alt="image" title="image" />
          <input type="checkbox" class="checkbox popup" name="xhas_attachment_image" id="xhas_attachment_image" <?php if(isset($has_attachment_image) && $has_attachment_image == 1) { ?>checked="checked"<?php } ?> onclick="Piler.clear_attachment_any();" />

          <img src="view/theme/default/assets/images/fileicons/file.png" alt="any" title="any" />
          <input type="checkbox" class="checkbox popup" name="xhas_attachment_any" id="xhas_attachment_any" <?php if(isset($has_attachment_any) && $has_attachment_any == 1) { ?>checked="checked"<?php } ?> onclick="Piler.clear_attachment_others();" />
        </td>
      </tr>

      <tr>
        <td><?php print $text_date_from; ?></td>
        <td><input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" placeholder="<?php print DATE_FORMAT; ?>" /></td>
      </tr>

      <tr>
        <td><?php print $text_date_to; ?></td>
        <td><input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" placeholder="<?php print DATE_FORMAT; ?>" /></td>
      </tr>
    </table>

  </div>

  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="#" class="btn btn-primary" onclick="Piler.complex();" data-dismiss="modal" aria-hidden="true">OK</a>
  </div>
</div>

<?php if(Registry::get('auditor_user') == 1) { ?>
<div id="deletebox-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_delete_reason; ?></h3>
  </div>
  <div class="modal-body">
    <input type="text" id="reason" name="reason" />
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="#" onclick="var reason = $('#reason').val(); if(reason) { Piler.bulk_remove_messages(reason, '<?php if(NEED_TO_APPROVE_DELETE) { print $text_need_to_approve_removal; } else { print $text_successfully_removed; } ?>', '<?php print $text_no_selected_message; ?>'); }" class="btn btn-primary" data-dismiss="modal" aria-hidden="true">OK</a>
  </div>
</div>
<?php } ?>

<?php if( (OUTLOOK == 1 && SHOW_MENU_FOR_OUTLOOK == 1) || (OUTLOOK == 0 && MOBILE_DEVICE == 0) ) { ?>
    <div id="menu">
        <?php print $menu; ?>
    </div>
<?php } ?>

    <div id="messagebox1" class="alert alert-info lead"></div>

    <div id="piler1" class="container-fluid">

    <div id="searchcontainer">
         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />
         <input type="hidden" name="prefix" id="prefix" value="" />

         <div class="control-group">
            <div class="controls row-fluid">
                <div id="input-span" class="span6">
                    <label for="_search"><?php print $text_search; ?></label>
                <input type="text" id="_search" name="_search" placeholder="<?php print $text_enter_search_terms; ?>" />
                </div>

             <?php if(MOBILE_DEVICE == 0) { ?>
                <div class="span6 input-append btn-group">
                    <button id="button_search" class="btn btn-large btn-danger" onclick="Piler.expert(this); return false;"><i class="icon-search icon-large"></i>&nbsp;<?php print $text_search; ?></button>
                    <button id="button_expert" class="btn btn-large btn-inverse" onclick="Piler.show_advanced_search_modal();"><?php print $text_advanced_search; ?> &nbsp;<span class="caret"></span></button>
                    <button id="button_options" class="btn btn-large btn-inverse dropdown-toggle" data-toggle="dropdown"><?php print $text_options; ?> &nbsp;<span class="caret"></span></button>
                    <ul class="dropdown-menu">
                        <li><a href="#" onclick="Piler.saved_search_terms('<?php print $text_saved; ?>');"><?php print $text_save; ?></a></li>
                        <li><a href="#" onclick="Piler.load_saved_search_terms();"><?php print $text_load; ?></a></li>
                    </ul>
                </div>
             <?php } ?>
              </div>
            </div>
         </div>
    </div>

    <div id="mainscreen">
        <div id="mailleftcontainer">
        <?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
        <div id="mailboxlist-container">
        <?php print $folder; ?>
        </div>
        <script type="text/javascript">
          var mailviewsplitv = new rcube_splitter({id:'splitter1', p1: 'mailleftcontainer', p2: 'mailrightcontainer', orientation: 'v', relative: true, start: 165});
          split.add_onload('mailviewsplitv.init()');
        </script>
        <?php } ?>
        </div>
        <div id="mailrightcontainer<?php if(ENABLE_FOLDER_RESTRICTIONS == 0) { ?>nofolder<?php } ?>">

            <div id="mailrightcontent">
              <div id="mailcontframe">
                <div id="sspinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_working; ?></div>
                <div id="messagelistcontainer" class="boxlistcontent" style="top:0">

                <?php print $content; ?>

                </div>
              </div>

        <script type="text/javascript">
          var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: 341});
          split.add_onload('mailviewsplit.init()');
        </script>

              <div id="mailpreviewframe"></div>

          </div>

        </div>
    </div>

    <script type="text/javascript">
    $(document).ready(function(){
       split.init();
    });
    </script>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
