<?php if($page > 0) { ?>
  <a href="#" class="navlink" onclick="Piler.navigation(0);"><i class="bi bi-chevron-double-left"></i></a>
<?php } else { ?>
  <span class="navlink"><i class="bi bi-chevron-double-left muted"></i></span>
<?php } ?>

&nbsp;

<?php if($page > 0) { ?>
  <a href="#" class="navlink" onclick="Piler.navigation(<?php print $prev_page; ?>);"><i class="bi bi-chevron-left"></i></a>
<?php } else { ?>
  <span class="navlink"><i class="bi bi-chevron-left muted"></i></span>
<?php } ?>

&nbsp;

<?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $hits; ?><?php if($total_found > MAX_SEARCH_HITS) { ?> (<?php print $total_found; ?>)<?php } ?>

&nbsp;

<?php if($next_page <= $total_pages){ ?>
  <a href="#" class="navlink" onclick="Piler.navigation(<?php print $next_page; ?>);"><i class="bi bi-chevron-right"></i></a>
<?php } else { ?>
  <span class="navlink"><i class="bi bi-chevron-right muted"></i></span>
<?php } ?>

&nbsp;

<?php if($page < $total_pages) { ?>
  <a href="#" class="navlink" onclick="Piler.navigation(<?php print $total_pages; ?>);"><i class="bi bi-chevron-double-right"></i></a>
<?php } else { ?>
  <span class="navlink"><i class="bi bi-chevron-double-right muted"></i></span>
<?php } ?>

&nbsp;
