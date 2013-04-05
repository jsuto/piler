
<?php if(isset($errorstring)){ ?><div class="alert alert-error"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>

<?php if(isset($x)){ ?>

<div class="alert alert-success"><?php print $x; ?>.</div>
<p><a href="index.php?route=user/list"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<?php } else { ?>

<p><a href="index.php?route=user/list"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<form action="index.php?route=user/add" name="adduser" method="post" autocomplete="off" class="form-horizontal">

    <div class="control-group">
        <input type="hidden" name="uid" value="<?php print $next_user_id; ?>" />
        <label class="control-label" for="email"><?php print $text_email_addresses; ?>:</label>
		<div class="controls">
		  <textarea name="email"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea>
		</div>
	</div>
	
	<div class="control-group">
		<label class="control-label" for="username"><?php print $text_username; ?>:</label>
		<div class="controls">
		  <input type="text" name="username" value="<?php if(isset($post['username'])){ print $post['username']; } ?>" class="text" />
		</div>
	</div>
	
	<div class="control-group">
		<label class="control-label" for="realname"><?php print $text_realname; ?>:</label>
		<div class="controls">
		  <input type="text" name="realname" value="<?php if(isset($post['realname'])){ print $post['realname']; } ?>" class="text" />
		</div>
	</div>	
	
	<div class="control-group">
		<label class="control-label" for="domain"><?php print $text_domain; ?>:</label>
		<div class="controls">
		  <select name="domain">
               <?php asort($domains); foreach ($domains as $domain) { ?>
                  <option value="<?php if(is_array($domain)){ print $domain['mapped']; } else { print $domain; } ?>"<?php if( (isset($post) && $domain == $post['domain']) || (!isset($post) && isset($_SESSION['last_domain']) && $domain == $_SESSION['last_domain']) ){ ?> selected="selected"<?php } ?>><?php if(is_array($domain)){ print $domain['mapped']; } else { print $domain; } ?></option>
               <?php } ?>
          </select>
		</div>
	</div>	
	
	<div class="control-group">
		<label class="control-label" for="s_piler_domains"><?php print $text_search_domains; ?>*:</label>
		<div class="controls">
		  <input type="text" id="s_piler_domain" name="s_piler_domain" placeholder="<?php print $text_search_domain_to_add; ?>" class="autocompletetext" />
		</div>
	</div>

	<div class="control-group">
		<label class="control-label" for="group"><?php print $text_domains; ?>**:</label>
		<div class="controls">
		  <textarea style="height:80px;" name="domains" id="domains"><?php if(isset($post['domains'])){ print $post['domains']; } ?></textarea>
		</div>
	</div>    
    
	<div class="control-group">
		<label class="control-label" for="s_piler_group"><?php print $text_search_groups; ?>*:</label>
		<div class="controls">
		  <input type="text" id="s_piler_group" name="s_piler_group" placeholder="<?php print $text_search_group_to_add; ?>" class="autocompletetext" />
		</div>
	</div>

	<div class="control-group">
		<label class="control-label" for="group"><?php print $text_groups; ?>**:</label>
		<div class="controls">
		  <textarea style="height:80px;" name="group" id="group"><?php if(isset($post['group'])){ print $post['group']; } ?></textarea>
		</div>
	</div>

	<div class="control-group">
		<label class="control-label" for="s_piler_folder"><?php print $text_search_folders; ?>*:</label>
		<div class="controls">
		  <input type="text" id="s_piler_folder" name="s_piler_folder" placeholder="<?php print $text_search_folder_to_add; ?>" class="autocompletetext" />
		</div>
	</div>

	<div class="control-group">
		<label class="control-label" for="folder"><?php print $text_folders; ?>**:</label>
		<div class="controls">
		  <textarea style="height:80px;" name="folder" id="folder"><?php if(isset($post['folder'])){ print $post['folder']; } ?></textarea>
		</div>
	</div>

<?php if(ENABLE_LDAP_IMPORT_FEATURE == 1) { ?>
	<div class="control-group">
		<label class="control-label" for="dn">LDAP DN:</label>
		<div class="controls">
		  <input type="text" name="dn" value="<?php if(isset($post['dn'])){ print $post['dn']; } ?>" class="text" /><br /> (<?php print $text_dn_asterisk_means_skip_sync; ?>)
		</div>
	</div>
<?php } ?>

	<div class="control-group">
		<label class="control-label" for="password"><?php print $text_password; ?>:</label>
		<div class="controls">
		  <input type="password" name="password" value="" class="text" />
		</div>
	</div>	

	<div class="control-group">
		<label class="control-label" for="password2"><?php print $text_password_again; ?>:</label>
		<div class="controls">
		  <input type="password" name="password2" value="" class="text" />
		</div>
	</div>	
	
	<div class="control-group">
		<label class="control-label" for="isadmin"><?php print $text_admin_user; ?>:</label>
		<div class="controls">
		  <select name="isadmin">
               <option value="0"<?php if(isset($post['isadmin']) && $post['isadmin'] == 0){ ?> selected="selected"<?php } ?>><?php print $text_user_regular; ?></option>
               <?php if(Registry::get('admin_user') == 1) { ?><option value="1"<?php if(isset($post['isadmin']) && $post['isadmin'] == 1){ ?> selected="selected"<?php } ?>><?php print $text_user_masteradmin; ?></option><?php } ?>
               <option value="2"<?php if(isset($post['isadmin']) && $post['isadmin'] == 2){ ?> selected="selected"<?php } ?>><?php print $text_user_auditor; ?></option>
               <option value="3"<?php if(isset($post['isadmin']) && $post['isadmin'] == 3){ ?> selected="selected"<?php } ?>><?php print $text_user_read_only_admin; ?></option>
          </select>
		</div>
	</div>	
	
	<div class="alert alert-info">*: <?php print $text_min_2_chars; ?><br />**: <?php print $text_enter_one_group_per_line; ?></div>
	
	<div class="form-actions">
        <input type="submit" value="<?php print $text_add; ?>" class="btn btn-primary" /> <a href="index.php?route=user/list" class="btn"><?php print $text_cancel; ?></a>
	</div>

</form>

<?php } ?>
