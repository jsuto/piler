<div>

<?php if($confirmed){ ?>

<div class="alert alert-success"><?php print $x; ?>.</div>
<p><a href="<?php print PATH_PREFIX; ?>index.php?route=domain/domain"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<?php } else { ?>

<p><a href="<?php print PATH_PREFIX; ?>index.php?route=domain/domain"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a> | <a href="<?php print PATH_PREFIX; ?>index.php?route=domain/remove&amp;domain=<?php print $domain; ?>&amp;confirmed=1"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove_this_user; ?>: <?php print $user; ?></a></p>

<?php } ?>

</div>
