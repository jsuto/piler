
<div class="alert alert-<?php if($n > 0) { ?>success<?php } else { ?>error<?php } ?>">

<?php print $text_domain; ?>: <?php print $domain; ?><br />
<?php print $text_retention_days; ?>: <?php print $days; ?><br />
<?php print $text_updated_records; ?>: <?php print $n; ?>

</div>

