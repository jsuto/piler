<div class="messageheader">
    <p>
       <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="icon-cloud-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
       <a class="messagelink" href="#" onclick="Piler.view_single_message(<?php print $id; ?>);"><i class="icon-envelope-alt"></i>&nbsp;<?php print $text_view_message; ?></a> |
       <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><i class="icon-envelope-alt"></i>&nbsp;<?php print $text_view_headers; ?></a>
    </p>
</div>

<div class="messagecontents">
<?php print $data; ?>
</div>

