
<p>

<?php if($confirmed){ ?>

<?php print $x; ?>. <a href="index.php?route=group/list"><?php print $text_back; ?></a>

<?php } else { ?>
<a href="index.php?route=group/remove&amp;id=<?php print $id; ?>&amp;group=<?php print $group; ?>&amp;confirmed=1"><?php print $text_remove_this_group; ?>: <?php print $group; ?></a>
<?php } ?>

</p>

