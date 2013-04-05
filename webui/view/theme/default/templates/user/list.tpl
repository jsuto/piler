<form method="post" name="search1" action="index.php?route=user/list" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<p><a href="index.php?route=user/add"><i class="icon-plus"></i>&nbsp;<?php print $text_add_new_user_alias; ?></a></p>

<h4><?php print $text_existing_users; ?></h4>

<?php if(isset($users)){ ?>

<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
   <?php print $users[0][$sort]; ?> - <?php print $users[count($users)-1][$sort]; ?>
   &nbsp;
   <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?><a href="index.php?route=user/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right"></i><?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?></a><?php } ?>
   &nbsp;
   <?php if($page < $total_pages){ ?><a href="index.php?route=user/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
</div>

   <table class="table table-striped table-condensed" id="ss1">
      <tr class="domainrow">
         <th><?php print $text_realname; ?> <a href="index.php?route=user/list&amp;sort=realname&amp;order=0"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=realname&amp;order=1"><i class="icon-chevron-down"></i></a></th>
         <th><?php print $text_email; ?> <a href="index.php?route=user/list&amp;sort=email&amp;order=0"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=email&amp;order=1"><i class="icon-chevron-down"></i></a></th>
         <th><?php print $text_role; ?> <a href="index.php?route=user/list&amp;sort=role&amp;order=0"><i class="icon-chevron-up"></i> <a href="index.php?route=user/list&amp;sort=role&amp;order=1"><i class="icon-chevron-down"></i></a></th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($users as $user) { ?>
      <tr class="domainrow">
         <td class="domaincell"><?php print $user['realname']; ?></td>
         <td class="domaincell"><?php if($user['email'] != $user['shortemail']){ ?><span><?php print $user['shortemail']; ?></span><?php } else { print $user['email']; } ?></td>
         <td class="domaincell">
            <?php
               if($user['isadmin'] == 0){ print $text_user_regular; }
               if($user['isadmin'] == 1){ print $text_user_masteradmin; }
               if($user['isadmin'] == 2){ print $text_user_auditor; }
               if($user['isadmin'] == 3){ print $text_user_read_only_admin; }
            ?>
         </td>
         <td class="domaincell"><a href="index.php?route=user/edit&amp;uid=<?php print $user['uid']; ?>"><?php print $text_edit_or_view; ?></a></td>
      </tr>
<?php } ?>

   </table>


<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left"></i><?php if($page > 0){ ?></a><?php } ?>
   &nbsp;
   <?php print $users[0][$sort]; ?> - <?php print $users[count($users)-1][$sort]; ?>
   &nbsp;
   <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?><a href="index.php?route=user/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right"></i><?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?></a><?php } ?>
   &nbsp;
   <?php if($page < $total_pages){ ?><a href="index.php?route=user/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
</div>

<?php } else { ?>
<div class="alert alert-error lead">
<?php print $text_not_found; ?>
</div>
<?php } ?>


