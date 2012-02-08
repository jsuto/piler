<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
   <title>clapf web UI</title>
   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="en" />
   <meta name="keywords" content="clapf, webui, web ui, spam, anti-spam, email, e-mail, mail, unsolicited commercial bulk email, blacklist, software, filtering, service, Bayes, Bayesian" />
   <meta name="description" content="clapf, webui, web ui, spam, anti-spam, antispam" />
   <meta name="rating" content="general" />

   <meta name="robots" content="all" />

</head>

<body>

<p>
<form action="index.php?route=health/worker" method="post">
   <input type="hidden" name="confirmed" value="1" />
   <input type="hidden" name="resetcounters" value="1" />
   <input type="submit" name="submit" value="<?php print $text_confirm_to_reset_counters; ?>" />
</form>
</p>


</body>
</html>
