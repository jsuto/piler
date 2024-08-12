<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title><?php if($title_prefix) { print $title_prefix; ?> | <?php } ?><?php print $title; ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
    <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
    <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>

    <?php print CSS_CODE; ?>
    <?php print CUSTOM_CSS; ?>

    <?php print BRANDING_FAVICON; ?>
  </head>

  <body id="loginpage">

<?php if(!Registry::get('username')) { ?>

    <div id="logincontainer" class="container">
        <div id="logo-lg"><img src="<?php print SITE_LOGO_LG; ?>" alt="Archive Logo Image" title="Login" /></div>

        <form name="login" action="index.php?route=login/foureyes" method="post" class="form-signin">

            <h2 class="fs-1 mb-3 fw-light">4eyes <?php print $text_login; ?></h2>

            <?php if(isset($x)){ ?><p class="alert alert-error lead"><?php print $x; ?></p><?php } ?>
            <input type="hidden" name="relocation" value="<?php if(isset($_GET['route']) && !preg_match("/^login/", $_GET['route']) ) { if(isset($_SERVER['REDIRECT_URL'])) { print $_SERVER['REDIRECT_URL']; } else { print $_SERVER['QUERY_STRING']; } } ?>" />
            <div class="form-floating">
              <input name="username" type="email" class="form-control" id="username" placeholder="name@example.com" required autofocus>
              <label for="floatingInput"><?php print $text_email; ?></label>
            </div>
            <div class="form-floating">
              <input name="password" type="password" class="form-control" id="password" placeholder="Password">
              <label for="floatingPassword"><?php print $text_password; ?></label>
            </div>

        <?php if(CAPTCHA_FAILED_LOGIN_COUNT > 0 && $failed_login_count > CAPTCHA_FAILED_LOGIN_COUNT) { ?>
            <img src="securimage/securimage_show.php" alt="captcha image" id="captcha" />
            <input type="text" class="input-block-level" name="captcha" placeholder="CAPTCHA" />
        <?php } ?>

            <button class="btn btn-large btn-primary w-100 mt-3 py-2" type="submit" value="<?php print $text_submit; ?>"><?php print $text_submit; ?></button>
            <button class="btn btn-large w-100 mt-3 py-2" type="reset" value="<?php print $text_cancel; ?>" onclick="document.location.href='logout.php';"><?php print $text_cancel; ?></button>

        </form>

   </div>

<?php } ?>

<!-- <?php print PILER_LOGIN_HELPER_PLACEHOLDER; ?> -->

  <?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

  </body>
</html>
