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

   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/jquery.ui.all.css" />

   <script type="text/javascript" src="/view/javascript/jquery-1.7.1.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery-ui-1.8.13.custom.min.js"></script>
   <script type="text/javascript" src="/view/javascript/jquery.dropdownPlain.js"></script>

   <script type="text/javascript" src="/view/javascript/piler.js"></script>

</head>

<body>

<div id="wrap">

  <div id="menu">

    <?php print $menu; ?>

  </div> <!-- menu -->

      <div id="main">

<?php if($title) { ?><h3><?php print $title; ?></h3><?php } ?>
<?php print $content; ?>

      </div> <!-- main -->


   <div id="footer">
<?php print $footer; ?>
   </div>


</div> <!-- wrap -->

</body>
</html>
