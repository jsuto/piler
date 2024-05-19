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
  <?php print CUSTOM_CSS; ?>

  <!-- Fav and touch icons -->
  <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
  <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
  <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
  <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
  <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body>

<?php include_once DIR_THEME . THEME . '/templates/common/common.tpl'; ?>

<div id="menu">
    <?php print $menu; ?>
</div>

<div class="searchcontainer text-center">

  <div id="main" class="container">
      <?php if($title) { ?>

      <div class="container text-start">
        <div class="row">
          <h2><?php print $title; ?></h2>
        </div>
      </div>

      <?php } ?>
      <?php print $content; ?>

  </div> <!-- main -->

  <div id="footer"><?php print $footer; ?></div>


</div>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

<?php print JS_CODE; ?>
<script type="text/javascript" src="/view/javascript/fileupload.js"></script>

</body>
</html>
