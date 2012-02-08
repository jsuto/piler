
<p>&nbsp;</p>

<p>
<table border="1">
<?php while(list($k, $v) = each($counters)) { ?>
   <tr><td><?php print $k; ?></td><td><?php print $v; ?></td></tr>
<?php } ?>
   <?php if($counters[$prefix . 'rcvd'] > 0) { ?><tr><td>spam / <?php print $text_total_ratio; ?></td><td><?php print sprintf("%.2f", 100*$counters[$prefix . 'spam'] / $counters[$prefix . 'rcvd']); ?> %</td></tr><?php } ?>
   <?php if($counters[$prefix . 'rcvd'] > 0) { ?><tr><td>virus / <?php print $text_total_ratio; ?></td><td><?php print sprintf("%.2f", 100*$counters[$prefix . 'virus'] / $counters[$prefix . 'rcvd']); ?> %</td></tr><?php } ?>

</table>
</p>

<p>
<form action="index.php?route=stat/counter" method="post">
   <input type="hidden" name="reset" value="1" />
   <input type="submit" name="submit" value="<?php print $text_reset_counters; ?>" />
</form>
</p>
