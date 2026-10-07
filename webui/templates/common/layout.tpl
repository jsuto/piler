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

  <?php print BRANDING_FAVICON; ?>
</head>

<body>

<?php include_once DIR_BASE . 'templates/common/common.tpl'; ?>

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
<script type="text/javascript" src="<?php print PATH_PREFIX; ?>assets/js/fileupload.js"></script>

</body>
</html>
