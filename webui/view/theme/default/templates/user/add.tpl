
<?php if(isset($errorstring)){ ?><p class="loginfailed"><?php print $text_error; ?>: <?php print $errorstring; ?></p><?php } ?>


<form action="index.php?route=user/add" name="adduser" method="post" autocomplete="off">
   <input type="hidden" name="uid" value="<?php print $next_user_id; ?>" />

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_email_addresses; ?>:</div>
         <div class="domaincell"><textarea name="email" class="domain"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_username; ?>:</div>
         <div class="domaincell"><input type="text" name="username" value="<?php if(isset($post['username'])){ print $post['username']; } ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_realname; ?>:</div>
         <div class="domaincell"><input type="text" name="realname" value="<?php if(isset($post['realname'])){ print $post['realname']; } ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_domain; ?>:</div>
         <div class="domaincell">
            <select name="domain">
               <?php asort($domains); foreach ($domains as $domain) { ?>
                  <option value="<?php if(is_array($domain)){ print $domain['mapped']; } else { print $domain; } ?>"<?php if( (isset($post) && $domain == $post['domain']) || (!isset($post) && isset($_SESSION['last_domain']) && $domain == $_SESSION['last_domain']) ){ ?> selected="selected"<?php } ?>><?php if(is_array($domain)){ print $domain['mapped']; } else { print $domain; } ?></option>
               <?php } ?>
            </select>
         </div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_search_groups; ?>*:</div>
         <div class="domaincell"><input type="text" id="s_piler_group" name="s_piler_group" value="<?php print $text_search_group_to_add; ?>" class="autocompletetext" onfocus="javascript:toggle_hint('s_piler_group', '<?php print $text_search_group_to_add; ?>', 1);" onblur="javascript:toggle_hint('s_piler_group', '<?php print $text_search_group_to_add; ?>', 0);" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_groups; ?>**:</div>
         <div class="domaincell"><textarea style="height:80px;" name="group" id="group" class="domain"><?php if(isset($post['group'])){ print $post['group']; } ?></textarea></div>
      </div>


      <div class="domainrow">
         <div class="domaincell"><?php print $text_search_folders; ?>*:</div>
         <div class="domaincell"><input type="text" id="s_piler_folder" name="s_piler_folder" value="<?php print $text_search_folder_to_add; ?>" class="autocompletetext" onfocus="javascript:toggle_hint('s_piler_folder', '<?php print $text_search_folder_to_add; ?>', 1);" onblur="javascript:toggle_hint('s_piler_folder', '<?php print $text_search_folder_to_add; ?>', 0);" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_folders; ?>**:</div>
         <div class="domaincell"><textarea style="height:80px;" name="folder" id="folder" class="domain"><?php if(isset($user['folder'])){ print $user['folder']; } ?></textarea></div>
      </div>  


<?php if(ENABLE_LDAP_IMPORT_FEATURE == 1) { ?>
      <div class="domainrow">
         <div class="domaincell">LDAP DN:</div>
         <div class="domaincell"><input type="text" name="dn" value="<?php print $user['dn']; ?>" class="text" /><br /> (<?php print $text_dn_asterisk_means_skip_sync; ?>)</div>
      </div>
<?php } ?>


      <div class="domainrow">
         <div class="domaincell"><?php print $text_password; ?>:</div>
         <div class="domaincell"><input type="password" name="password" value="" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_password_again; ?>:</div>
         <div class="domaincell"><input type="password" name="password2" value="" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_admin_user; ?>:</div>
         <div class="domaincell">
            <select name="isadmin">
               <option value="0"<?php if(isset($user['isadmin']) && $user['isadmin'] == 0){ ?> selected="selected"<?php } ?>><?php print $text_user_regular; ?></option>
               <?php if(Registry::get('admin_user') == 1) { ?><option value="1"<?php if(isset($user['isadmin']) && $user['isadmin'] == 1){ ?> selected="selected"<?php } ?>><?php print $text_user_masteradmin; ?></option><?php } ?>
               <option value="2"<?php if(isset($user['isadmin']) && $user['isadmin'] == 2){ ?> selected="selected"<?php } ?>><?php print $text_user_auditor; ?></option>
               <option value="3"<?php if(isset($user['isadmin']) && $user['isadmin'] == 3){ ?> selected="selected"<?php } ?>><?php print $text_user_read_only_admin; ?></option>
            </select>
         </div>
      </div>

      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell">*: <?php print $text_min_2_chars; ?><br />**: <?php print $text_enter_one_group_per_line; ?></div>
      </div>

      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_add; ?>" /><input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>



</form>


<?php if(isset($x)){ print $x; ?>. <a href="index.php?route=user/list"><?php print $text_back; ?></a>
<?php } ?>

