<div>

<?php if($confirmed){ ?>

<div class="alert alert-success"><?php print $x; ?>.</div>
<p><a href="index.php?route=group/list"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<?php } else { ?>

<p><a href="index.php?route=group/edit&amp;id=<?php print $id; ?>"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a> | <a href="index.php?route=group/remove&amp;id=<?php print $id; ?>&amp;group=<?php print $group; ?>&amp;confirmed=1"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove_this_group; ?>: <?php print $group; ?></a></p>

<?php } ?>

</div>