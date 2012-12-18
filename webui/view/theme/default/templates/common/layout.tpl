<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title>piler | <?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="en" />
   <meta name="keywords" content="piler email archiver" />
   <meta name="description" content="piler email archiver" />
   <meta name="rating" content="general" />
   <meta name="robots" content="all" />

   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/jquery.ui.all.css" />

   <script type="text/javascript" src="/view/javascript/jquery-1.7.1.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery-ui-1.8.13.custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery.dropdownPlain.js"></script>

   <script type="text/javascript">
      var piler_ui_lang = '<?php if(LANG == 'en') { ?>en-GB<?php } else { print LANG; } ?>';
      var email_search_url = '<?php print SITE_URL; ?>index.php?route=group/email&';
      var group_search_url = '<?php print SITE_URL; ?>index.php?route=group/group&';
      var folder_search_url = '<?php print SITE_URL; ?>index.php?route=folder/folder&';
      var health_worker_url = "<?php print HEALTH_WORKER_URL; ?>";
   </script>

   <script type="text/javascript" src="/view/javascript/piler.js"></script>

</head>

<body<?php if(isset($this->request->get['route']) && $this->request->get['route'] == 'health/health') { ?> onload="loadHealth(); setInterval('loadHealth()', <?php print HEALTH_REFRESH; ?> * 1000)" style="cursor: wait"<?php } ?>>

<div id="fuku1">

   <div id="menu"><?php print $menu; ?></div>

   <div id="main" align="center" style="border: 0px solid red; text-align: left; margin-left: 20px; margin-right: 20px; width: 1160px;">

      <?php if($title) { ?><h3><?php print $title; ?></h3><?php } ?>

      <?php print $content; ?>

   </div>

   <div id="footer"><?php print $footer; ?></div>


</div>

</body>
</html>
