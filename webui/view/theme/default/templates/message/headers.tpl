<html>
<head>
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
</head>

<body style="background: white;">

<div id="messagepopup">

<p>
   <a class="messagelink" href="/index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
   <a class="messagelink" href="/index.php?route=message/restore&amp;id=<?php print $id; ?>"><?php print $text_restore_to_mailbox; ?></a> |
   <a class="messagelink" href="/message.php/<?php print $id; ?>"><?php print $text_view_message; ?></a>
<?php if(Registry::get('admin_user') == 1 && HOLD_EMAIL == 0) { ?>
 | <a class="messagelink" href="/index.php?route=message/remove&amp;id=<?php print $id; ?>"><?php print $text_remove_message; ?></a>
<?php } ?>
</p>

<pre><?php print $data; ?></pre>

</div>

</body>
</html>
