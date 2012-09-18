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

   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/mini.css" />

   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/jquery.ui.all.css" />


   <script type="text/javascript" src="/view/javascript/jquery-1.7.1.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery-ui-1.8.13.custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery.dropdownPlain.js"></script>

   <script type="text/javascript">
      var current_message_id = 0;
      var piler_ui_lang = '<?php if(LANG == 'en') { ?>en-GB<?php } else { print LANG; } ?>';
      var email_search_url = '<?php print SITE_URL; ?>/index.php?route=group/email&';
      var group_search_url = '<?php print SITE_URL; ?>/index.php?route=group/group&';
      var folder_search_url = '<?php print SITE_URL; ?>/index.php?route=folder/folder&';
      var folder_copy_url = '<?php print SITE_URL; ?>/index.php?route=folder/copy';
      var message_loader_url = '<?php print SITE_URL; ?>message.php/';
   </script>

   <script type="text/javascript" src="/view/javascript/piler.js"></script>
   <script type="text/javascript" src="/view/javascript/splitter.js"></script>
   <script type="text/javascript" src="/view/javascript/rc1.js"></script>
   <script type="text/javascript" src="/view/javascript/rc2.js"></script>

   <script type="text/javascript" src="/view/javascript/shortcut.js"></script>

   <style type="text/css">
      /*#mailcontframe { height: 341px; }
      #mailpreviewframe { top: 351px; }
      #mailleftcontainer { width: 160px; }
      #mailrightcontainer { left: 170px; min-height: 200px; }*/
   </style>

   <script type="text/javascript">
      var split = new rcube_webmail();

      shortcut.add("Down", function() {
         move_message_list_scrollbar('down');
      },{
	'type':'keydown',
	'propagate':true,
	'target':document
      });

      shortcut.add("Up", function() {
        move_message_list_scrollbar('up');
      },{
        'type':'keydown',
        'propagate':true,
        'target':document
      });

      shortcut.add("Enter", function() {
        var a=document.getElementById('ref'); if(a) a.value=''; load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0);
      },{
        'type':'keydown',
        'propagate':true,
        'target':document
      });


   </script>


<script>

</script>

</head>

<body <?php if($open_saved_search_box == 1) { ?> onload="load_ajax_url('<?php print SAVE_SEARCH_URL; ?>&<?php print $search_args; ?>');" style="cursor: wait"<?php } ?>>

<div id="messagebox1" style="position: absolute; top: 0px; left: 50%; border: 3px solid red; display: none; background: orange; color: #000000; font-weight: bold; padding: 15px; z-index: 1;"></div>

<div id="fuku1">


<div id="menu">
<?php print $menu; ?>
</div>

<div style="position: absolute; border: 1px solid black; right: 20px; left: 20px; text-align: right; ">

         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />

         <span style="font-weight: bold;"><?php print $text_search; ?>:</span>

         <input type="text" id="_search" name="_search" class="advtext<?php if(!isset($_search)) { ?>grey<?php } ?>" style="width: 528px;margin: 0px;" value="<?php if(isset($_search)) { print $_search; } else { print $text_enter_search_terms; ?>" onclick="if(expsrc==0){this.value=''; this.className = 'advtext'; expsrc++;}<?php } ?>" />

         <div class="aoo" onclick="javascript: document.getElementById('searchpopup1').style.display = ''; return false;">
            <div class="aoq"></div>
         </div>

         <button id="button_search" class="active" style="margin-left: 10px; margin-right: 0px; height: 20px; width: 70px;" onclick="script:var a=document.getElementById('ref'); if(a) a.value=''; load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0);"><?php print $text_search; ?></button>
         <input type="button" class="advsecondary" style="height: 20px; width: 70px;" onclick="javascript:var a=document.getElementById('_search'); a.value=''; a = document.getElementById('ref'); a.value=''; return false;" value="<?php print $text_cancel; ?>" />
         <input type="button" class="advsecondary" style="height: 20px; width: 70px;" value="<?php print $text_save; ?>" onclick="javascript:send_ajax_post_request('<?php print SAVE_SEARCH_URL; ?>', assemble_search_term(0, '') + '&save=1'); show_message('messagebox1', '<p><?php print $text_saved; ?></p>', 0.85);" />
         <input type="button" class="advsecondary" style="height: 20px; width: 70px;" value="<?php print $text_load; ?>" onclick="javascript:load_saved_search_terms('<?php print LOAD_SAVED_SEARCH_URL; ?>');" />


         <?php print $popup; ?>


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

  <div id="mailrightcontainer"<?php if(ENABLE_FOLDER_RESTRICTIONS == 0) { ?> style="left: 0px;"<?php } ?>>
    <div id="mailrightcontent">

      <div id="mailcontframe">

        <div id="messagelistcontainer" class="boxlistcontent" style="top:0; height:auto;">
<?php print $content; ?>
        </div>
      </div>

<script type="text/javascript">
  var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: 205});
  split.add_onload('mailviewsplit.init()');
</script>

      <div id="mailpreviewframe" onmouseover="javascript: current_message_id = 0;" style="text-align: left; overflow:auto;">
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
