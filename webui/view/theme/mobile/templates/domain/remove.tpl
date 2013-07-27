
<p>

<?php if($confirmed){ ?>

<?php print $x; ?>. <a href="index.php?route=domain/domain"><?php print $text_back; ?></a>

<?php } else { ?>
<a href="index.php?route=domain/remove&amp;domain=<?php print $domain; ?>&amp;confirmed=1"><?php print $text_remove_domain; ?>: <?php print $domain; ?></a>
<?php } ?>

</p>

