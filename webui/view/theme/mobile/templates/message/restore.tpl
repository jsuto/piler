<html>
<head>
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
</head>

<body>

<div id="messagepopup">

<p>
   <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="icon-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
   <a class="messagelink" href="#" onclick="Piler.view_message(<?php print $id; ?>);"><i class="icon-envelope"></i>&nbsp;<?php print $text_view_message; ?></a> |
   <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><i class="icon-envelope"></i>&nbsp;<?php print $text_view_headers; ?></a>
<?php if($message['has_journal'] == 1 && Registry::get('auditor_user') == 1 && SHOW_ENVELOPE_JOURNAL == 1) { ?>
   | <a class="messagelink" href="#" onclick="Piler.view_journal(<?php print $id; ?>);"><i class="icon-envelope"></i>&nbsp;<?php print $text_view_journal_envelope; ?></a>
<?php } ?>
</p>

<p><?php print $data; ?></p>

</div>

</body>
</html>
