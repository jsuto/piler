
<p/>

<p><a href="index.php?route=user/add"><?php print $text_add_new_user_alias; ?></a></p>

<h4><?php print $text_existing_users; ?></h4>

<form method="post" name="search1" action="index.php?route=user/list">
   <input type="text" name="search" value="<?php print $search; ?>" />
   <input type="submit" value="<?php print $text_search; ?>" />
</form>


<p>&nbsp;</p>

<?php if(isset($users)){ ?>

<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php print $users[0][$sort]; ?> - <?php print $users[count($users)-1][$sort]; ?>
   <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?><a href="index.php?route=user/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?></a><?php } ?>
   <?php if($page < $total_pages){ ?><a href="index.php?route=user/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
</div>


   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_realname; ?> <a href="index.php?route=user/list&amp;sort=realname&amp;order=0"><img src="<?php print ICON_ARROW_UP; ?>" border="0"></a> <a href="index.php?route=user/list&amp;sort=realname&amp;order=1"><img src="<?php print ICON_ARROW_DOWN; ?>" border="0"></a></div>
         <div class="domaincell"><?php print $text_email; ?> <a href="index.php?route=user/list&amp;sort=email&amp;order=0"><img src="<?php print ICON_ARROW_UP; ?>" border="0"></a> <a href="index.php?route=user/list&amp;sort=email&amp;order=1"><img src="<?php print ICON_ARROW_DOWN; ?>" border="0"></a></div>
         <div class="domaincell"><?php print $text_role; ?> <a href="index.php?route=user/list&amp;sort=domain&amp;order=0"><img src="<?php print ICON_ARROW_UP; ?>" border="0"></a> <a href="index.php?route=user/list&amp;sort=domain&amp;order=1"><img src="<?php print ICON_ARROW_DOWN; ?>" border="0"></a></div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($users as $user) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $user['realname']; ?></div>
         <div class="domaincell"><?php if($user['email'] != $user['shortemail']){ ?><span><?php print $user['shortemail']; ?></span><?php } else { print $user['email']; } ?></div>
         <div class="domaincell">
            <?php
               if($user['isadmin'] == 0){ print $text_user_regular; }
               if($user['isadmin'] == 1){ print $text_user_masteradmin; }
               if($user['isadmin'] == 2){ print $text_user_auditor; }
               if($user['isadmin'] == 3){ print $text_user_read_only_admin; }
            ?>
         </div>
         <div class="domaincell"><a href="index.php?route=user/edit&amp;uid=<?php print $user['uid']; ?>"><?php print $text_edit_or_view; ?></a></div>
      </div>
<?php } ?>

   </div>


<div id="pagenav">
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=0&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php if($page > 0){ ?><a href="index.php?route=user/list&amp;page=<?php print $prev_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
   <?php print $users[0][$sort]; ?> - <?php print $users[count($users)-1][$sort]; ?>
   <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?><a href="index.php?route=user/list&amp;page=<?php print $next_page; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total_users >= $page_len*($page+1) && $total_users > $page_len){ ?></a><?php } ?>
   <?php if($page < $total_pages){ ?><a href="index.php?route=user/list&amp;page=<?php print $total_pages; ?>&amp;search=<?php print $search; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
</div>


<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>


