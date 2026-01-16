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

    <div style="position: fixed; top: 20px; right: 20px; z-index: 1000;">
      <button id="theme-toggle" class="btn" style="background: transparent; border: 1px solid #ccc; padding: 8px 12px;" title="Toggle Dark Mode" aria-label="Toggle Dark Mode">
        <i class="bi bi-moon-fill" id="theme-icon" style="font-size: 20px;"></i>
      </button>
    </div>

    <div id="logincontainer" class="container">
        <div id="logo-lg"><img src="<?php print SITE_LOGO_LG; ?>" alt="Archive Logo Image" title="Login" /></div>

        <form name="login" action="<?php print PATH_PREFIX; ?>login.php" method="post" class="form-signin">

            <h2 class="fs-1 mb-3 fw-light"><?php print $text_login; ?></h2>

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

        </form>

        <div class="container text-center">
          <div class="row align-items-start">
            <div class="col"></div>
            <div class="col mt-2 mb-3 text-body-secondary text-bg-light compatibility">
            <?php if(ENABLE_GOOGLE_LOGIN == 1) { ?>
               <p><a href="<?php print $auth_url; ?>"><?php print $text_login_via_google; ?></a></p>
            <?php } ?>

              <p><?php print LOGIN_EXTRA_NOTES; ?></p>
            </div>
            <div class="col"></div>
          </div>
        </div>



   </div>

<?php } ?>

<!-- <?php print PILER_LOGIN_HELPER_PLACEHOLDER; ?> -->

  <?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

  <script>
    function getScreenSize() {
      let width = window.screen.width
      let height = window.screen.height

      document.cookie = "SCREEN_X=" + width + "; path=/"
      document.cookie = "SCREEN_Y=" + height + "; path=/"
    }

    window.onload = getScreenSize
  </script>
  <script src="<?php print PATH_PREFIX; ?>assets/js/dark-mode.js"></script>

  </body>
</html>
