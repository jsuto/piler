<div>

<?php if($confirmed){ ?>

<div class="alert alert-success"><?php print $x; ?>.</div>
<p><a href="index.php?route=policy/legalhold"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<?php } else { ?>

<p><a href="index.php?route=policy/legalhold"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a> | <a href="index.php?route=policy/removehold&amp;email=<?php print $email; ?>&amp;confirmed=1"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove_this_user; ?>: <?php print $email; ?></a></p>

<?php } ?>

</div>
