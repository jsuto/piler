<!DOCTYPE html>
<html lang="<?php print DEFAULT_LANG; ?>">

<head>
    <title><?php print $title; ?></title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="<?php print DEFAULT_LANG; ?>" />
    <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
    <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
    <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
    <meta name="rating" content="general" />
    <meta name="robots" content="all" />

    <?php print CSS_CODE; ?>

    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>

    <?php print JS_CODE; ?>

    <script type="text/javascript" src="/view/javascript/jscolor.js"></script>
    <script type="text/javascript" src="/view/javascript/fileupload.js"></script>
</head>

<body<?php

if(isset($this->request->get['route'])) {

   if($this->request->get['route'] == 'health/health') { ?> onload="Piler.load_health(); setInterval('Piler.load_health()', Piler.health_refresh * 1000);"<?php }
   if($this->request->get['route'] == 'stat/online') { ?> onload="setInterval('Piler.reload_page()', Piler.health_refresh * 1000);"<?php }
   if($this->request->get['route'] == 'import/jobs') { ?> onload="setInterval('Piler.reload_page()', Piler.health_refresh * 1000);"<?php }

} ?>>

<div id="menu">
    <?php print $menu; ?>
</div>

<div id="messagebox1" class="alert alert-info lead"></div>

<div class="searchcontainer text-center" style="border: 0px solid red;">

  <div id="main" class="container" style="border: 2px solid black; ">
    <div class="col text-start">
      <?php if($title) { ?><h3><?php print $title; ?></h3><?php } ?>
      <?php print $content; ?>
    </div>

  </div> <!-- main -->

  <div id="footer"><?php print $footer; ?></div>


</div>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
