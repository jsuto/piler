
<h4><?php print $text_add_new_group; ?></h4>

<?php if(isset($errorstring)){ ?><p class="loginfailed"><?php print $text_error; ?>: <?php print $errorstring; ?></p><?php } ?>


<form action="index.php?route=group/add" name="adduser" method="post" autocomplete="off">

   <div id="ss1">

      <div class="domainrow">
         <div class="domaincell"><?php print $text_groupname; ?>:</div>
         <div class="domaincell"><input type="text" name="groupname" value="<?php if(isset($post['groupname'])){ print $post['groupname']; } ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_email_addresses; ?>*:</div>
         <div class="domaincell"><textarea style="height:280px;" name="email" id="email" class="domain"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea></div>
      </div>

      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell">*: <?php print $text_enter_one_email_address_per_line; ?></div>
      </div>


      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_add; ?>" /><input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>



</form>


<?php if(isset($x)){ print $x; ?>. <a href="index.php?route=group/list"><?php print $text_back; ?></a>
<?php } ?>

