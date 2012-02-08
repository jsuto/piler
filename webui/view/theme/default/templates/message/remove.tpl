<html>
<head>
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style.css" />
</head>

<body style="background: white;">

<div id="messagepopup">

<p>
   <a class="messagelink" href="/index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
   <a class="messagelink" href="/index.php?route=message/restore&amp;id=<?php print $id; ?>"><?php print $text_restore_to_mailbox; ?></a> |
   <a class="messagelink" href="/message.php/<?php print $id; ?>"><?php print $text_view_message; ?></a> |
   <a class="messagelink" href="/index.php?route=message/headers&amp;id=<?php print $id; ?>"><?php print $text_view_headers; ?></a>
</p>

<p><?php print $data; ?></p>

</div>

</body>
</html>
