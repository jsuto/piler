
<p>

<?php if($confirmed){ ?>

<?php print $x; ?>. <a href="index.php?route=policy/folder"><?php print $text_back; ?></a>

<?php } else { ?>
<a href="index.php?route=policy/removefolder&amp;id=<?php print $id; ?>&amp;confirmed=1"><?php print $text_remove_rule; ?></a>: <?php

if($rule['domain']) { print $text_domain . ': ' . $rule['domain'] . ', '; }
if($rule['from']) { print $text_from . ': ' . $rule['from'] . ', '; }
if($rule['to']) { print $text_to . ': ' . $rule['to'] . ', '; }
if($rule['subject']) { print $text_subject . ': ' . $rule['subject'] . ', '; }
if($rule['body']) { print $text_body . ': ' . $rule['body'] . ', '; }
if($rule['size'] > 0) { print $text_size . ': ' . $rule['_size'] . ' ' . $rule['size'] . ', '; }
if($rule['attachment_name']) { print $text_attachment_name . ': ' . $rule['attachment_name'] . ', '; }
if($rule['attachment_type']) { print $text_attachment_type . ': ' . $rule['attachment_type'] . ', '; }
if($rule['attachment_size'] > 0) { print $text_attachment_size . ': ' . $rule['_attachment_size'] . ' ' . $rule['attachment_size'] . ' '; }
?> <a href="index.php?route=policy/folder"><?php print $text_back; ?></a>

<?php } ?>

</p>

