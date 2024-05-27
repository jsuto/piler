<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title><?php if($title_prefix) { print $title_prefix; ?> | <?php } ?><?php print $title; ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
    <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
    <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
    <meta http-equiv="refresh" content="5; url=<?php print SITE_URL; ?>">

    <?php print CSS_CODE; ?>
    <?php print CUSTOM_CSS; ?>

    <?php print BRANDING_FAVICON; ?>
  </head>

<body id="loginpage">

  <div id="logincontainer" class="container">
      <h3><?php print $title; ?></h3>
      <p><?php print $text_logged_out; ?>. <a href="<?php print PATH_PREFIX; if(ENABLE_SSO_LOGIN == 1) { ?>sso.php<?php } else { ?>login.php<?php } ?>" class="messagelink"><?php print $text_back; ?></a></p>
  </div>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
