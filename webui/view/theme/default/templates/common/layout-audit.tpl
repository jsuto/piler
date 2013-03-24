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

         <input type="text" id="_search" name="_search" class="advtextgrey" value="" placeholder="<?php print $text_enter_search_terms; ?>" />

         <button id="button_search" class="active" onclick="Piler.auditexpert(this);"><?php print $text_search; ?></button>
         <input type="button" class="advsecondary" onclick="Piler.cancel();" value="<?php print $text_cancel; ?>" />

         <div id="sspinner">
            <img src="/view/theme/default/images/spinner.gif" id="spinner" alt="spinner" />
         </div>

</div>

<div id="resultsheader">
<div id="resultstop">
   <div class="resultrow">
      <div class="auditcell date header">
         <?php print $text_date; ?>
         <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="auditcell user header">
         <?php print $text_user; ?>
         <a xid="user" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="user" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="auditcell ip header">
         <?php print $text_ipaddr; ?>
         <a xid="ipaddr" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="ipaddr" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="auditcell action header">
         <?php print $text_action; ?>
         <a xid="action" xorder="1" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
         <a xid="action" xorder="0" onclick="Piler.changeOrder(this);"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
      </div>
      <div class="auditcell description header">
         <?php print $text_description; ?>
      </div>
      <div class="auditcell ref header">
         <?php print $text_ref; ?>
      </div>

   </div>
</div>
</div>


<div id="mainscreen">

  <div id="mailleftcontainer">
  </div>

  <div id="mailrightcontainer<?php if(ENABLE_FOLDER_RESTRICTIONS == 0) { ?>nofolder<?php } ?>">

    <div id="mailrightcontent">

      <div id="mailcontframe">

        <div id="messagelistcontainer" class="boxlistcontent"> 

              <?php print $content; ?>

        </div>
      </div>

  </div>


</div>


</div>

</body>
</html>
