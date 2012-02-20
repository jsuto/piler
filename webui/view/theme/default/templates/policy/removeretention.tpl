
<p>

<?php if($confirmed){ ?>

<?php print $x; ?>. <a href="index.php?route=policy/retention"><?php print $text_back; ?></a>

<?php } else { ?>
<a href="index.php?route=policy/removeretention&amp;id=<?php print $id; ?>&amp;confirmed=1"><?php print $text_remove_rule; ?></a>: <?php

if($rule['from']) { print $text_from . ': ' . $rule['from'] . ', '; }
if($rule['to']) { print $text_to . ': ' . $rule['to'] . ', '; }
if($rule['subject']) { print $text_subject . ': ' . $rule['subject'] . ', '; }
if($rule['size'] > 0) { print $text_size . ': ' . $rule['_size'] . ' ' . $rule['size'] . ', '; }
if($rule['attachment_type'] > 0) { print $text_attachment_type . ': ' . $rule['attachment_type'] . ', '; }
if($rule['attachment_size'] > 0) { print $text_attachment_size . ': ' . $rule['_attachment_size'] . ' ' . $rule['attachment_size'] . ' '; }
?> <a href="index.php?route=policy/retention"><?php print $text_back; ?></a>

<?php } ?>

</p>

