<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title>piler | <?php print $title; ?></title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="en" />
   <meta name="keywords" content="clapf, webui, web ui, spam, anti-spam, email, e-mail, mail, unsolicited commercial bulk email, blacklist, software, filtering, service, Bayes, Bayesian" />
   <meta name="description" content="clapf, webui, web ui, spam, anti-spam, antispam" />
   <meta name="rating" content="general" />
   <meta name="robots" content="all" />

   <link rel="stylesheet" type="text/css" href="view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />

   <script type="text/javascript" src="view/javascript/jquery-1.7.1.min.js"></script>
   <script type="text/javascript" src="view/javascript/jquery.dropdownPlain.js"></script>

<script type="text/javascript">
   var health_worker_url = "<?php print HEALTH_WORKER_URL; ?>";
</script>

</head>

<body onload="loadHealth(); setInterval('loadHealth()', <?php print HEALTH_REFRESH; ?> * 1000)" style="cursor: wait">

   <script type="text/javascript" src="view/javascript/wz_tooltip.js"></script>
   <script type="text/javascript" src="view/javascript/tip_balloon.js"></script>

   <script type="text/javascript" src="view/javascript/piler.js"></script>



<div id="wrap">

  <div id="menu">
    <ul id="menulist">

    <?php print $menu; ?>

    </ul>

  </div> <!-- menu -->

      <div id="main">

<h3><?php print $title; ?></h3>
<?php print $content; ?>

      </div> <!-- main -->

   <div id="footer">
<?php print $footer; ?>
   </div>


</div> <!-- wrap -->

</body>
</html>
