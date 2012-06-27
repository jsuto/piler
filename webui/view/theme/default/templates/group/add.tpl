
<?php if(isset($errorstring)){ ?><p class="loginfailed"><?php print $text_error; ?>: <?php print $errorstring; ?></p><?php } ?>


<form action="index.php?route=group/add" name="adduser" method="post" autocomplete="off">

   <div id="ss1">

      <div class="domainrow">
         <div class="domaincell"><?php print $text_groupname; ?>:</div>
         <div class="domaincell"><input type="text" name="groupname" value="<?php if(isset($post['groupname'])){ print $post['groupname']; } ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_search_emails; ?>*:</div>
         <div class="domaincell"><input type="text" id="s_piler_email" name="s_piler_email" value="<?php print $text_search_email_to_add; ?>" class="autocompletetext" onfocus="javascript:toggle_hint('s_piler_email', '<?php print $text_search_email_to_add; ?>', 1);" onblur="javascript:toggle_hint('s_piler_email', '<?php print $text_search_email_to_add; ?>', 0);" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_email_addresses; ?>**:</div>
         <div class="domaincell"><textarea style="height:280px;" name="email" id="email" class="domain"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea></div>
         <div class="domaincell">
<?php foreach(Registry::get('letters') as $letter) { ?>
         <a href="#" onclick="window.open('<?php print SITE_URL; ?>index.php?route=group/email&term=<?php print $letter; ?>', 'aaa', 'width=300,height=400');" ><?php print $letter; ?></a>
<?php } ?>
         </div>
      </div>

      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell">*: <?php print $text_min_2_chars; ?><br />**: <?php print $text_enter_one_email_address_per_line; ?></div>
      </div>


      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_add; ?>" /><input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>



</form>


<?php if(isset($x)){ print $x; ?>. <a href="index.php?route=group/list"><?php print $text_back; ?></a>
<?php } ?>

