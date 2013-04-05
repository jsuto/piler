
<?php foreach($domains as $domain) { ?>
   <a href="#" onclick="append_value_from_slider('domains', '<?php print $domain['domain']; ?>');"><?php print $domain['domain']; ?></a><br />
<?php } ?>


<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=domain/domains&amp;page=0&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
   <?php if($page > 0){ ?><a href="index.php?route=domain/domains&amp;page=<?php print $prev_page; ?>&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
   <?php if(count($domains) > 0) { print $domains[0][$sort]; ?> - <?php print $domains[count($domains)-1][$sort]; } ?>
    &nbsp;
   <?php if($total >= $page_len*($page+1) && $total > $page_len){ ?><a href="index.php?route=domain/domains&amp;page=<?php print $next_page; ?>&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total >= $page_len*($page+1) && $total > $page_len){ ?></a><?php } ?>
    &nbsp;
   <?php if($page < $total_pages){ ?><a href="index.php?route=domain/domains&amp;page=<?php print $total_pages; ?>&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
</div>

<div class="top20px"><a href="#" onclick="window.close();">close</a></div>
