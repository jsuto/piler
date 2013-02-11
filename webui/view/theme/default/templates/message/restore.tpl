<html>
<head>
   <link rel="stylesheet" type="text/css" href="/view/theme/default/stylesheet/style-<?php print THEME; ?>.css" />
</head>

<body>

<div id="messagepopup">

<p>
   <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
   <a class="messagelink" href="#" onclick="Piler.view_message(<?php print $id; ?>);"><?php print $text_view_message; ?></a> |
   <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><?php print $text_view_headers; ?></a>
</p>

<p><?php print $data; ?></p>

</div>

</body>
</html>
