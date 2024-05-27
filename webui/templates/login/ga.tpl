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


<body class="loginpage">


    <div id="logincontainer" class="container">
        <div id="logo-lg"><img src="<?php print SITE_LOGO_LG; ?>" alt="Archive Logo Image" title="<?php print $text_login; ?>" /></div>

        <form name="login" action="<?php print PATH_PREFIX; ?>index.php?route=login/ga" method="post" class="form-signin" autocomplete="off">

            <h2 class="form-signin-heading"><?php print $text_enter_google_authenticator_code; ?></h2>

            <?php if(isset($x)){ ?><p class="alert alert-error lead"><?php print $x; ?></p><?php } ?>
            <input type="hidden" name="relocation" value="<?php if(isset($_GET['route']) && !preg_match("/^login/", $_GET['route']) ) { if(isset($_SERVER['REDIRECT_URL'])) { print $_SERVER['REDIRECT_URL']; } else { print $_SERVER['QUERY_STRING']; } } ?>" />

            <input type="password" class="input-block-level bold" id="ga_code" name="ga_code" placeholder="<?php print $text_google_authenticator_code; ?>" required autofocus>

            <button class="btn btn-large btn-primary" type="submit" value="<?php print $text_submit; ?>"><?php print $text_submit; ?></button>
            <button class="btn btn-large" type="reset" value="<?php print $text_cancel; ?>" onclick="document.location.href='logout.php';"><?php print $text_cancel; ?></button>

        </form>

   </div>



<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
