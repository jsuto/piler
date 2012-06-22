

<?php if(isset($user)) {

   $userbasedn = preg_replace("/cn=([\w]+),/", "", $user['dn']); ?>

<form action="index.php?route=user/edit" name="adduser" method="post" autocomplete="off">
   <input type="hidden" name="uid" value="<?php print $uid; ?>" />

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_email_addresses; ?>:</div>
         <div class="domaincell"><textarea name="email" class="domain"><?php print $emails; ?></textarea></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_username; ?>:</div>
         <div class="domaincell"><input type="text" name="username" value="<?php print $user['username']; ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_realname; ?>:</div>
         <div class="domaincell"><input type="text" name="realname" value="<?php print $user['realname']; ?>" class="text" /></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_domain; ?>:</div>
         <div class="domaincell">
            <select name="domain">
               <?php asort($domains); foreach ($domains as $domain) { ?>
                  <option value="<?php print $domain; ?>"<?php if($domain == $user['domain']){ ?> selected="selected"<?php } ?>><?php print $domain; ?></option>
               <?php } ?>
            </select>
         </div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_groups; ?>:</div>
         <div class="domaincell">
            <select name="gid">
               <option value="0"<?php if($user['gid'] == 0){ ?> selected="selected"<?php } ?>>-</option>
<?php foreach ($groups as $group) { ?>
               <option value="<?php print $group['id']; ?>"<?php if($user['gid'] == $group['id']){ ?> selected="selected"<?php } ?>><?php print $group['groupname']; ?></option>
<?php } ?>
            </select>
         </div>
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
         <div class="domaincell"><?php print $text_user_id; ?>:</div>
         <div class="domaincell"><?php print $uid; ?></div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_admin_user; ?>:</div>
         <div class="domaincell">
            <select name="isadmin">
               <option value="0"<?php if($user['isadmin'] == 0){ ?> selected="selected"<?php } ?>><?php print $text_user_regular; ?></option>
               <?php if(Registry::get('admin_user') == 1) { ?><option value="1"<?php if($user['isadmin'] == 1){ ?> selected="selected"<?php } ?>><?php print $text_user_masteradmin; ?></option><?php } ?>
               <option value="2"<?php if($user['isadmin'] == 2){ ?> selected="selected"<?php } ?>><?php print $text_user_auditor; ?></option>
               <option value="3"<?php if($user['isadmin'] == 3){ ?> selected="selected"<?php } ?>><?php print $text_user_read_only_admin; ?></option>
            </select>
         </div>
      </div>

      <div class="domainrow">
         <div class="domaincell"><?php print $text_group_membership; ?>:</div>
         <div class="domaincell">
            <?php foreach ($user['group_membership'] as $_group_uid) { ?>
               <?php $a = preg_split("/\s/", $this->model_user_user->getEmailsByUid($_group_uid)); print $a[0]; ?></br />
            <?php } ?>
         </div>
      </div>

      <div class="domainrow">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_modify; ?>" /><input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>


</form>

<p>&nbsp;</p>
<p><a href="index.php?route=user/remove&amp;uid=<?php print $user['uid']; ?>&amp;user=<?php print $user['username']; ?>"><?php print $text_remove_this_user; ?>: <?php print $user['username']; ?></a></p>
<p>&nbsp;</p>

<p>
<?php } else if(isset($x)){ print $x; ?>. 
<?php } ?>

<a href="index.php?route=user/list"><?php print $text_back; ?></a>
</p>
