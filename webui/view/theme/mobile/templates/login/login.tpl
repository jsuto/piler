<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title><?php if($title_prefix) { print $title_prefix; ?> | <?php } ?><?php print $title; ?></title>
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
   <script type="text/javascript" src="/view/javascript/jquery-ui-custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/bootstrap.min.js"></script>
   <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>
   <script type="text/javascript" src="/view/javascript/piler.js"></script>

   <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body class="loginpage">
   
<?php if(!Registry::get('username')) { ?>

    <div id="logincontainer" class="container">
        <div id="logo-lg"><img src="<?php print SITE_LOGO_LG; ?>" alt="Archive Logo Image" title="<?php print $text_login; ?>" /></div>

        <form name="login" action="login.php" method="post" class="form-signin">

            <h2 class="form-signin-heading"><?php print $text_login; ?></h2>

            <?php if(isset($x)){ ?><p class="alert alert-error lead"><?php print $x; ?></p><?php } ?>
            <input type="hidden" name="relocation" value="<?php if(isset($_GET['route']) && !preg_match("/^login/", $_GET['route']) ) { if(isset($_SERVER['REDIRECT_URL'])) { print $_SERVER['REDIRECT_URL']; } else { print $_SERVER['QUERY_STRING']; } } ?>" />

            <input type="text" class="input-block-level" name="username" placeholder="<?php print $text_email; ?>" required autofocus>
            <input type="password" class="input-block-level bold" name="password" placeholder="<?php print $text_password; ?>">

         <?php if(CAPTCHA_FAILED_LOGIN_COUNT > 0 && $failed_login_count > CAPTCHA_FAILED_LOGIN_COUNT) { ?>
            <img src="securimage/securimage_show.php" alt="captcha image" id="captcha" />
            <input type="text" class="input-block-level" name="captcha" placeholder="CAPTCHA" />
         <?php } ?>

            <button class="btn btn-large btn-primary" type="submit" value="<?php print $text_submit; ?>"><?php print $text_submit; ?></button>

        </form>

        <div id="compatibility" class="well well-large">

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
