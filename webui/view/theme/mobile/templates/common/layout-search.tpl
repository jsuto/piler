<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title><?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="<?php print DEFAULT_LANG; ?>" />
   <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
   <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
   <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
   <meta name="rating" content="general" />
   <meta name="robots" content="all" />

   <meta name="viewport" content="width=device-width, initial-scale=1.0">

   <link href="/bootstrap/css/bootstrap<?php print BOOTSTRAP_THEME; ?>.min.css" rel="stylesheet" media="screen">
   <link href="/bootstrap/css/bootstrap-responsive.min.css" rel="stylesheet" media="screen">

   <link rel="stylesheet" type="text/css" href="/view/theme/<?php print THEME; ?>/stylesheet/jquery-ui-custom.min.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/<?php print THEME; ?>/stylesheet/style-<?php print THEME; ?>.css" />

   <script type="text/javascript" src="/view/javascript/jquery.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery-ui-custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/bootstrap.min.js"></script>
   <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>

<?php if(ENABLE_TABLE_RESIZE == 1) { ?>
   <script type="text/javascript" src="/view/javascript/store.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery.resizableColumns.min.js"></script>
<?php } ?>

   <script type="text/javascript" src="/view/javascript/piler.js"></script>

   <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body class="mybody" onload="Piler.add_shortcuts();">

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
    <a href="#" onclick="var reason =  $('#reason').val(); if(reason) { alert(reason); }" class="btn btn-primary" data-dismiss="modal" aria-hidden="true">OK</a>
  </div>
</div>
<?php } ?>

<div id="messagebox1"></div>

<div id="piler1" class="container">

<?php if(!(OUTLOOK == 1 && SHOW_MENU_FOR_OUTLOOK == 0)) { ?>
<div id="menu">
   <?php print $menu; ?>
</div>
<?php } ?>

<div id="expertsearch">

      <form class="well form-search sleek">

         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />
         <input type="hidden" name="prefix" id="prefix" value="" />

         <input type="text" id="_search" name="_search" class="input-medium span6" value="" placeholder="<?php print $text_enter_search_terms; ?>" />

      <?php if(OUTLOOK == 0) { ?>
         <a id="advsearch_caret" href="#" onclick="$('#searchpopup1').show();"><b class="caret"></b></a>
      <?php } ?>

         <button id="button_search" class="btn btn-danger" onclick="Piler.expert(this); return false;"><?php print $text_search; ?></button>

         <button class="btn" onclick="Piler.cancel(); return false;"><?php print $text_cancel; ?></button>
         <button class="btn " onclick="Piler.saved_search_terms('<?php print $text_saved; ?>'); return false;"><?php print $text_save; ?></button>
         <button class="btn btn-inverse" onclick="Piler.load_saved_search_terms(); return false;"><?php print $text_load; ?></button>
      </form>

         <?php if(OUTLOOK == 0) { print $popup; } ?>

         <div id="sspinner">
            <div class="progress progress-striped active">
               <div class="bar" style="width: 100%;"></div>
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

        <div id="messagelistcontainer" class="boxlistcontent"> 

<?php print $content; ?>

        </div>
      </div>

<script type="text/javascript">
  var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: 205});
  split.add_onload('mailviewsplit.init()');
</script>

      <div id="mailpreviewframe">
      </div>


  </div>



</div>


</div>


<script type="text/javascript">
$(document).ready(function(){
   split.init();
   Piler.fix_page();
});
</script>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
