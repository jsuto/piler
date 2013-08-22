<table class="table table-striped table-condensed">
<?php foreach($emails as $email) { ?>
   <tr><td><a href="#" onclick="append_value_from_slider('email', '<?php print $email; ?>');"><?php print $email; ?></a></td></tr>
<?php } ?>
</table>

<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=group/email&amp;page=0&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
   <?php if($page > 0){ ?><a href="index.php?route=group/email&amp;page=<?php print $prev_page; ?>&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
   <?php if(count($emails) > 0) { print $emails[0][$sort]; ?> - <?php print $emails[count($emails)-1][$sort]; } ?>
   &nbsp;
   <?php if($total >= $page_len*($page+1) && $total > $page_len){ ?><a href="index.php?route=group/email&amp;page=<?php print $next_page; ?>&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right"></i><?php if($total >= $page_len*($page+1) && $total > $page_len){ ?></a><?php } ?>
   &nbsp;
   <?php if($page < $total_pages){ ?><a href="index.php?route=group/email&amp;page=<?php print $total_pages; ?>&amp;term=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
</div>

<div style="margin-top: 20px;"><a href="#" onclick="window.close();"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_close;?></a></div>
