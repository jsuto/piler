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
   <script type="text/javascript" src="/view/javascript/piler.js"></script>

</head>

<body style="background-color: white;">

<div id="wrap" style="width:200px; height: 300px; border: 0px;">

<?php if($title) { ?><h3><?php print $title; ?></h3><?php } ?>
<?php print $content; ?>


</div> <!-- wrap -->

</body>
</html>
