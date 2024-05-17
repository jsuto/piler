<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">
    </div>
    <div class="col text-end">
      <form method="post" name="search1" action="index.php?route=user/list">
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

      <h4><?php print $text_add_new_entry; ?> <a href="index.php?route=user/add"><i class="bi bi-plus"></i></a></h4>

      <table class="table table-striped">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_realname; ?> <a href="index.php?route=user/list&amp;sort=realname&amp;order=0<?php print $querystring; ?>"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=realname&amp;order=1<?php print $querystring; ?>"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_username; ?> <a href="index.php?route=user/list&amp;sort=username&amp;order=0<?php print $querystring; ?>"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=username&amp;order=1<?php print $querystring; ?>"<?php print $querystring; ?>><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_email; ?> <a href="index.php?route=user/list&amp;sort=email&amp;order=0"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=email&amp;order=1<?php print $querystring; ?>"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_role; ?> <a href="index.php?route=user/list&amp;sort=role&amp;order=0"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=role&amp;order=1<?php print $querystring; ?>"><i class="icon-chevron-down"></i></a></th>
            <th>&nbsp;</th>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($users as $user) { ?>
          <tr class="domainrow">
            <td><?php print $user['realname']; ?></td>
            <td><?php print $user['username']; ?></td>
            <td><?php if($user['email'] != $user['shortemail']){ ?><span><?php print $user['shortemail']; ?></span><?php } else { print $user['email']; } ?></td>
            <td>
            <?php
               if($user['isadmin'] == 0){ print $text_user_regular; }
               if($user['isadmin'] == 1){ print $text_user_masteradmin; }
               if($user['isadmin'] == 2){ print $text_user_auditor; }
               if($user['isadmin'] == 4){ print $text_user_data_officer; }
            ?>
            </td>
            <td><a href="index.php?route=user/edit&amp;uid=<?php print $user['uid']; ?>"><i class="bi bi-pencil-square" title="<?php print $text_edit_or_view; ?>"></i></a></td>
            <td><a href="index.php?route=user/remove&amp;id=<?php print $user['uid']; ?>&amp;user=<?php print $user['username']; ?>&amp'confirmed=1"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></i></a></td>
          </tr>
        <?php } ?>
      </table>

    <?php if($total_users > $page_len) { ?>
      <div id="pagenav">
      <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-double-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
      <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-left"></i><?php if($page > 0){ ?></a><?php } ?>
      &nbsp;
      <?php print $users[0][$sort]; ?> - <?php print $users[count($users)-1][$sort]; ?>
      &nbsp;
      <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?><a href="index.php?route=user/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-right"></i><?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?></a><?php } ?>
      &nbsp;
      <?php if($page < $total_pages){ ?><a href="index.php?route=user/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-double-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
      </div>
    <?php } ?>

    </div>
  </div>
</div>
