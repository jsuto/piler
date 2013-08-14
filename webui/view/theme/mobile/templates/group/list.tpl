<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_group_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=group/remove&amp;id=-1&amp;name=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<p/>

<p><a href="index.php?route=group/add"><?php print $text_add_new_group; ?></a></p>

<h4><?php print $text_existing_groups; ?></h4>

<form method="post" name="search1" class="form-search" action="index.php?route=group/list">
   <input type="text" name="search" value="<?php print $search; ?>" />
   <input type="submit" class="btn btn-primary" value="<?php print $text_search; ?>" />
</form>


<p>&nbsp;</p>

<?php if(isset($groups) && count($groups) > 0){ ?>

<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php print $groups[0][$sort]; ?> - <?php print $groups[count($groups)-1][$sort]; ?>
   <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?><a href="index.php?route=group/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?></a><?php } ?>
   <?php if($page < $total_pages){ ?><a href="index.php?route=group/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
</div>


   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_groupname; ?> <a href="index.php?route=group/list&amp;sort=groupname&amp;order=0"><img src="<?php print ICON_ARROW_UP; ?>" border="0"></a> <a href="index.php?route=group/list&amp;sort=groupname&amp;order=1"><img src="<?php print ICON_ARROW_DOWN; ?>" border="0"></a></div>
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($groups as $group) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $group['groupname']; ?></div>
         <div class="domaincell"><a href="index.php?route=group/edit&amp;id=<?php print $group['id']; ?>"><?php print $text_edit_or_view; ?></a></div>
         <div class="domaincell"><a href="index.php?route=group/remove&amp;id=<?php print $group['id']; ?>&amp;name=<?php print urlencode($group['groupname']); ?>" class="confirm-delete" data-id="<?php print $group['id']; ?>" data-name="<?php print $group['groupname']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></div>
      </div>
<?php } ?>


   </div>


<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php print $groups[0][$sort]; ?> - <?php print $groups[count($groups)-1][$sort]; ?>
   <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?><a href="index.php?route=group/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?></a><?php } ?>
   <?php if($page < $total_pages){ ?><a href="index.php?route=group/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
</div>


<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>


