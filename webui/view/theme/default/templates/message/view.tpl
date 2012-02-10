<html>
<head>
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
</head>

<body style="background: white;">

<div id="messagepopup">

<p>
   <a class="messagelink" href="/index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
   <a class="messagelink" href="/index.php?route=message/restore&amp;id=<?php print $id; ?>"><?php print $text_restore_to_mailbox; ?></a> |
   <a class="messagelink" href="/index.php?route=message/headers&amp;id=<?php print $id; ?>"><?php print $text_view_headers; ?></a>
<?php if(Registry::get('admin_user') == 1 && HOLD_EMAIL == 0) { ?>
|   <a class="messagelink" href="/index.php?route=message/remove&amp;id=<?php print $id; ?>"><?php print $text_remove_message; ?></a>
<?php } ?>
</p>

<strong><?php if($message['subject'] == "" || $message['subject'] == "Subject:") { print "&lt;" . $text_no_subject . "&gt;"; } else { print $message['subject']; } ?></strong><br />
<strong><?php print $message['from']; ?></strong><br />
<strong><?php print $message['to']; ?></strong><br />
<strong><?php print $message['date']; ?></strong><br />

<form action="/index.php?route=message/view" method="post" name="tag">
   <input type="hidden" name="id" value="<?php print $id; ?>" />
   <p><?php print $text_tags; ?>: <input type="text" name="tag" id="tag" value="<?php print $message['tag']; ?>" size="50" /> <input type="submit" value="<?php print $text_submit; ?>" /></p>
</form>

<hr />

<?php print $message['message']; ?><br />

</div>

</body>
</html>
