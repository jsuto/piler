<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title>piler | <?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="en" />
   <meta name="keywords" content="piler email archiver" />
   <meta name="description" content="piler email archiver" />
   <meta name="rating" content="general" />
   <meta name="robots" content="all" />

   <meta name="viewport" content="width=device-width, initial-scale=1.0">

   <link href="/bootstrap/css/bootstrap.min.css" rel="stylesheet" media="screen">
   <link href="/bootstrap/css/bootstrap-responsive.min.css" rel="stylesheet" media="screen">

   <link rel="stylesheet" type="text/css" href="/view/theme/<?php print THEME; ?>/stylesheet/jquery-ui-custom.min.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/<?php print THEME; ?>/stylesheet/style-<?php print THEME; ?>.css" />

   <script type="text/javascript" src="/view/javascript/jquery.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery-ui-custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/bootstrap.min.js"></script>
   <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>
   <script type="text/javascript" src="/view/javascript/piler.js"></script>

</head>

<body class="mybody">

<div id="piler1" class="container">


<?php if(!isset($_SESSION['username'])){ ?>

<div class="login">

   <?php if(ENABLE_GOOGLE_LOGIN == 1) { ?>
      <p><a href="<?php print $auth_url; ?>"><?php print $text_login_via_google; ?></a></p>
   <?php } else { ?>

      <form name="login" action="login.php" method="post" class="form-inline">
         <input type="text" class="input-medium" name="username" placeholder="<?php print $text_email; ?>" />
         <input type="password" class="input-medium" name="password" placeholder="<?php print $text_password; ?>" />
         <input type="submit" class="btn btn-primary" value="<?php print $text_submit; ?>" />
      </form>
   <?php } ?>


<?php if(isset($x)){ ?>
    <p class="text-error bold"><?php print $x; ?></p>
<?php } ?>


</div>

<?php } ?>


</div>

</body>
</html>
