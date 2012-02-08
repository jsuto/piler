<html>
<head>
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style.css" />
</head>

<body style="background: white;">

<div id="messagepopup">

<p>
   <a class="messagelink" href="/index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
   <a class="messagelink" href="/message.php/<?php print $id; ?>"><?php print $text_view_message; ?></a> |
   <a class="messagelink" href="/index.php?route=message/headers&amp;id=<?php print $id; ?>"><?php print $text_view_headers; ?></a>
<?php if(Registry::get('admin_user') == 1 && HOLD_EMAIL == 0) { ?>
 | <a class="messagelink" href="/index.php?route=message/remove&amp;id=<?php print $id; ?>"><?php print $text_remove_message; ?></a>
<?php } ?>
</p>

<p><?php print $data; ?></p>

</div>

</body>
</html>
