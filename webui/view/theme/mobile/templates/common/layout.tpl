<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title><?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="en" />
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
   <script type="text/javascript" src="/view/javascript/piler.js"></script>

   <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>

</head>

<body<?php

if(isset($this->request->get['route'])) {

   if($this->request->get['route'] == 'health/health') { ?> onload="Piler.load_health(); setInterval('Piler.load_health()', Piler.health_refresh * 1000);"<?php }
   if($this->request->get['route'] == 'stat/online') { ?> onload="setInterval('Piler.reload_page()', Piler.health_refresh * 1000);"<?php }
   if($this->request->get['route'] == 'import/jobs') { ?> onload="setInterval('Piler.reload_page()', 10 * 1000);"<?php }

} ?>>

<div id="piler1" class="container">

   <div id="menu">
      <?php print $menu; ?>
   </div>

   <div id="mainscreen">

      <?php if($title) { ?><h3 class="title"><?php print $title; ?></h3><?php } ?>

      <?php print $content; ?>

   </div> <!-- main -->

   <div id="footer"><?php print $footer; ?></div>


</div>

</body>
</html>
