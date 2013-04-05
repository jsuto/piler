<form method="post" name="search1" action="index.php?route=group/list" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<p><a href="index.php?route=group/add"><i class="icon-plus"></i>&nbsp;<?php print $text_add_new_group; ?></a></p>

<h4><?php print $text_existing_groups; ?></h4>

<?php if(isset($groups) && count($groups) > 0){ ?>

<div class="pagenav">
    <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
    <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
    <?php print $groups[0][$sort]; ?> - <?php print $groups[count($groups)-1][$sort]; ?>
    &nbsp;
    <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?><a href="index.php?route=group/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right"></i><?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?></a><?php } ?>
    &nbsp;
    <?php if($page < $total_pages){ ?><a href="index.php?route=group/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
</div>

    <table class="table table-striped table-condensed" id="ss1">
      <thead>
      <tr class="domainrow">
         <th class="domaincell"><?php print $text_groupname; ?> <a href="index.php?route=group/list&amp;sort=groupname&amp;order=0"><i class="icon-chevron-up"></i></a>&nbsp;<a href="index.php?route=group/list&amp;sort=groupname&amp;order=1"><i class="icon-chevron-down"></i></a></th>
         <th class="domaincell">&nbsp;</th>
      </tr>
      </thead>
      <tbody>
<?php foreach($groups as $group) { ?>
      <tr class="domainrow">
         <td class="domaincell"><?php print $group['groupname']; ?></td>
         <td class="domaincell"><a href="index.php?route=group/edit&amp;id=<?php print $group['id']; ?>"><?php print $text_edit_or_view; ?></a></td>
      </tr>
<?php } ?>
      </tbody>
   </table>

<div class="pagenav">
    &nbsp;
    <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
    <?php if($page > 0){ ?><a href="index.php?route=group/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
    <?php print $groups[0][$sort]; ?> - <?php print $groups[count($groups)-1][$sort]; ?>
    &nbsp;
    <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?><a href="index.php?route=group/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right"></i><?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?></a><?php } ?>
    &nbsp;
    <?php if($page < $total_pages){ ?><a href="index.php?route=group/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
    &nbsp;
</div>

<?php } else { ?>
<div class="alert alert-error lead">
<?php print $text_not_found; ?>
</div>
<?php } ?>


