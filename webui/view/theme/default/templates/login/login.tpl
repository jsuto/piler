<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title><?php if($title_prefix) { print $title_prefix; ?> | <?php } ?><?php print $title; ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
    <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
    <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>

    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
    <link href="/view/theme/default/assets/css/1.css" rel="stylesheet">

    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
  </head>

  <body id="loginpage">

<?php if(!Registry::get('username')) { ?>

    <div id="logincontainer" class="container">
        <div id="logo-lg"><img src="<?php print SITE_LOGO_LG; ?>" alt="Archive Logo Image" title="Login" /></div>

        <form name="login" action="login.php" method="post" class="form-signin">

            <h2 class="h3 mb-3 fw-normal"><?php print $text_login; ?></h2>

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

            <button class="btn btn-large btn-primary w-100 py-2" type="submit" value="<?php print $text_submit; ?>"><?php print $text_submit; ?></button>

        </form>

        <div class="mt-5 mb-3 text-body-secondary text-center">
            <?php if(ENABLE_GOOGLE_LOGIN == 1) { ?>
               <p><a href="<?php print $auth_url; ?>"><?php print $text_login_via_google; ?></a></p>
            <?php } ?>

            <p><?php print COMPATIBILITY; ?></p>
        </div>

   </div>

<?php } ?>

<!-- <?php print PILER_LOGIN_HELPER_PLACEHOLDER; ?> -->

  <?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

  </body>
</html>
