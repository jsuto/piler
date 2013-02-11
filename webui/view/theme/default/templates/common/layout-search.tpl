<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title><?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="en" />
   <meta name="keywords" content="piler email archiver" />
   <meta name="description" content="piler email archiver" />
   <meta name="rating" content="general" />
   <meta name="robots" content="all" />

   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/jquery-ui-custom.min.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/rc-splitter.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />

   <script type="text/javascript" src="/view/javascript/jquery.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery-ui-custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>
   <script type="text/javascript" src="/view/javascript/piler.js"></script>
</head>

<body onload="Piler.add_shortcuts();">

<div id="messagebox1"></div>

<div id="piler1">


<div id="menu">
<?php print $menu; ?>
</div>

<div id="expertsearch">

         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />
         <input type="hidden" name="prefix" id="prefix" value="" />

         <input type="text" id="_search" name="_search" class="advtextgrey" value="<?php print $text_enter_search_terms; ?>" onclick="Piler.toggle_search_class();" />

         <div class="aoo" onclick="$('#searchpopup1').show();">
            <div class="aoq"></div>
         </div>

         <button id="button_search" class="active" onclick="Piler.expert(this);"><?php print $text_search; ?></button>
         <input type="button" class="advsecondary" onclick="Piler.cancel();" value="<?php print $text_cancel; ?>" />
         <input type="button" class="advsecondary" onclick="Piler.saved_search_terms('<?php print $text_saved; ?>');" value="<?php print $text_save; ?>" />
         <input type="button" class="advsecondary" onclick="Piler.load_saved_search_terms();" value="<?php print $text_load; ?>" />


         <?php print $popup; ?>

         <div id="sspinner">
            <img src="/view/theme/default/images/spinner.gif" id="spinner" alt="spinner" />
         </div>

</div>

<div id="resultsheader">
  <div id="resultstop">
   <div class="resultrow">
      <div class="resultcell restore"><input type="checkbox" id="bulkcheck" name="bulkcheck" value="1" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" onclick="Piler.toggle_bulk_check();" /></div>
      <div class="resultcell id"><a href="#"><img class="download_icon" src="<?php print ICON_DOWNLOAD; ?>" width="18" height="18" alt="aaa" border="0" onclick="Piler.download_messages();" /></a></div>
      <div class="resultcell date header">
         <?php print $text_date; ?>
         <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="resultcell from header">
         <?php print $text_from; ?>
         <a xid="from" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="from" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="resultcell to header">
         <?php print $text_to; ?>
      </div>
      <div class="resultcell subject header">
         <?php print $text_subject; ?>
         <a xid="subj" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="subj" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="resultcell size header">
         <?php print $text_size; ?>
         <a xid="size" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="size" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="resultcell end">&nbsp;</div>
      <div class="resultcell end">&nbsp;</div>
      <div class="resultcell end">&nbsp;</div>
   <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
      <div class="resultcell verification">&nbsp;</div>
   <?php } ?>
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
});
</script>



</body>
</html>
