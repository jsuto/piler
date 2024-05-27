<div>

<?php if($confirmed){ ?>

<div class="alert alert-success"><?php print $x; ?>.</div>
<p><a href="<?php print PATH_PREFIX; ?>index.php?route=user/list"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<?php } else { ?>

<p><a href="<?php print PATH_PREFIX; ?>index.php?route=user/edit&amp;uid=<?php print $uid; ?>"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a> | <a href="<?php print PATH_PREFIX; ?>index.php?route=user/remove&amp;uid=<?php print $uid; ?>&amp;user=<?php print $user; ?>&amp;confirmed=1"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove_this_user; ?>: <?php print $user; ?></a></p>

<?php } ?>

</div>
