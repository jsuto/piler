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
   <script type="text/javascript" src="/view/javascript/jquery-ui.min.js"></script>
   <script type="text/javascript" src="/view/javascript/bootstrap.min.js"></script>
   <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>
   <script type="text/javascript" src="/view/javascript/piler.js"></script>

   <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body class="mybody" onload="Piler.add_shortcuts();">

<div id="messagebox1"></div>

<div id="piler1" class="container">


<div id="menu">
<?php print $menu; ?>
</div>

<div id="expertsearch">

      <form class="well form-search sleek">

         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />

         <input type="text" id="_search" name="_search" class="input-medium span6" value="" placeholder="<?php print $text_enter_search_terms; ?>" />

         <button id="button_search" class="btn btn-danger" onclick="Piler.auditexpert(this); return false;"><?php print $text_search; ?></button>
         <input type="button" class="btn" onclick="Piler.cancel();" value="<?php print $text_cancel; ?>" />
      </form>

         <div id="sspinner">
            <div class="progress progress-striped active">
               <div class="bar" style="width: 100%;"></div>
            </div>
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

  <div id="mailrightcontainernofolder">

    <div id="mailrightcontent">

      <div id="mailcontframe">

        <div id="messagelistcontainer" class="boxlistcontent"> 

<?php print $content; ?>

        </div>
      </div>

<?php if(Registry::get('auditor_user') == 1) { ?>

<script type="text/javascript">
  var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: 205});
  split.add_onload('mailviewsplit.init()');
</script>

      <div id="mailpreviewframe"></div>

<?php } ?>



  </div>



</div>


</div>

<?php if(Registry::get('auditor_user') == 1) { ?>

<script type="text/javascript">
$(document).ready(function(){
   split.init();
});
</script>

<?php } ?>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
