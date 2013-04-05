<p>

<?php if($timespan == "daily"){ ?>
<strong><?php print $text_daily_report; ?></strong> <a href="index.php?route=stat/stat&amp;timespan=monthly<?php if(isset($uid)) { ?>&amp;uid=<?php print $uid; } ?>"><?php print $text_monthly_report; ?></a>
<?php } else { ?>
<a href="index.php?route=stat/stat&amp;timespan=daily<?php if(isset($uid)) { ?>&amp;uid=<?php print $uid; } ?>"><?php print $text_daily_report; ?></a> <strong><?php print $text_monthly_report; ?></strong>
<?php } ?>

</p>

<p><img src="index.php?route=stat/graph&amp;timespan=<?php print $timespan; ?>&amp;uid=<?php print $uid; ?>" border="1" /> </p>

