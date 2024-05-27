<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">
    </div>
    <div class="col text-end">
      <form method="post" name="search1" action="<?php print PATH_PREFIX; ?>index.php?route=group/list">
        <div class="row justify-content-end">
          <div class="col-8">
            <input type="text" name="search" class="form-control" value="<?php print $search; ?>">
          </div>
          <div class="col-2">
            <button type="submit" class="btn btn-primary"><?php print $text_search; ?></button>
          </div>
        </div>
      </form>
    </div>
  </div>
</div>

<div class="container text-start mt-3">
  <div class="row">
    <div class="col">

      <h4><?php print $text_add_new_entry; ?> <a href="<?php print PATH_PREFIX; ?>index.php?route=group/add"><i class="bi bi-plus"></i></a></h4>

      <table class="table table-striped mt-4">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_groupname; ?> <a href="<?php print PATH_PREFIX; ?>index.php?route=group/list&amp;sort=groupname&amp;order=0"><i class="icon-chevron-up"></i></a>&nbsp;<a href="<?php print PATH_PREFIX; ?>index.php?route=group/list&amp;sort=groupname&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th>&nbsp;</th>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($groups as $group) { ?>
          <tr>
            <td><?php print $group['groupname']; ?></td>
            <td><a href="<?php print PATH_PREFIX; ?>index.php?route=group/edit&amp;id=<?php print $group['id']; ?>"><i class="bi bi-pencil-square" title="<?php print $text_edit_or_view; ?>"></i></a></td>
            <td><a href="<?php print PATH_PREFIX; ?>index.php?route=group/remove&amp;id=<?php print $group['id']; ?>&amp;confirmed=1"><i class="bi bi-trash text-danger"></i></a></td>
          </tr>
        <?php } ?>
        </tbody>
      </table>

<?php if($total_groups > $page_len) { ?>

<div class="pagenav">
    &nbsp;
    <?php if($page > 0){ ?><a href="<?php print PATH_PREFIX; ?>index.php?route=group/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-double-left"></i><?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
    <?php if($page > 0){ ?><a href="<?php print PATH_PREFIX; ?>index.php?route=group/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-left"></i><?php if($page > 0){ ?></a><?php } ?>
    &nbsp;
    <?php print $groups[0][$sort]; ?> - <?php print $groups[count($groups)-1][$sort]; ?>
    &nbsp;
    <?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?><a href="<?php print PATH_PREFIX; ?>index.php?route=group/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-right"></i><?php if($total_groups >= $page_len*($page+1) && $total_groups > $page_len){ ?></a><?php } ?>
    &nbsp;
    <?php if($page < $total_pages){ ?><a href="<?php print PATH_PREFIX; ?>index.php?route=group/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-double-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
    &nbsp;
</div>
<?php } ?>

    </div>
  </div>
</div>
