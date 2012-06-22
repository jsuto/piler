

<?php if(isset($group)) { ?>

<form action="index.php?route=group/edit" name="addgroup" method="post" autocomplete="off">
   <input type="hidden" name="id" value="<?php print $id; ?>" />

   <div id="ss1">

      <div class="domainrow">
         <div class="domaincell"><?php print $text_groupname; ?>:</div>
         <div class="domaincell"><input type="text" name="groupname" value="<?php print $group['groupname']; ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_email_addresses; ?>:</div>
         <div class="domaincell"><textarea style="height:280px;" name="email" class="domain"><?php if(isset($email)){ print $email; } ?></textarea></div>
      </div>

      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_modify; ?>" /><input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>


</form>

<p>&nbsp;</p>
<p><a href="index.php?route=group/remove&amp;id=<?php print $group['id']; ?>&amp;group=<?php print $group['groupname']; ?>"><?php print $text_remove_this_group; ?>: <?php print $group['groupname']; ?></a></p>
<p>&nbsp;</p>

<p>
<?php } else if(isset($x)){ print $x; ?>. 
<?php } ?>

<a href="index.php?route=group/list"><?php print $text_back; ?></a>
</p>
