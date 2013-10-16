
<?php if(!isset($x)){ ?>

	<h4><?php print $text_access_settings; ?></h4>
    <p><em><?php print $text_access_setting_explanation; ?></em></p>
    
    <table class="table table-striped">
	 <tr>
		<td class="span2"><?php print $text_email_addresses; ?>:</td>
		<td class="span8">
			<?php print $emails; ?>
		</td>
	 </tr>   
   
<?php if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) { ?>
   
	 <tr>
		<td><?php print $text_domains; ?>:</td>
		<td>
			<?php print $domains; ?>
		</td>
	 </tr>

<?php } ?>
	 
<?php if(Registry::get('auditor_user') == 0 || RESTRICTED_AUDITOR == 0) { ?>
	 
	 <tr>
		<td><?php print $text_groups; ?>:</td>
		<td>
			<?php print $groups; ?>
		</td>
	 </tr>

<?php } ?>
	 
<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
	 
	 <tr>
		<td><?php print $text_folders; ?>:</td>
		<td>
			<?php print $folders; ?>
		</td>
	 </tr>

<?php } ?>
	 
   </table>
   
<form action="/settings.php" method="post" name="setpagelen" class="form-horizontal">
   
	<h4><?php print $text_display_settings; ?></h4>

    <div class="control-group">
        <label class="control-label" for="pagelen"><?php print $text_page_length; ?></label>
        <div class="controls">
           <select name="pagelen">
              <option value="10"<?php if($page_len == 10) { ?> selected="selected"<?php } ?>>10
              <option value="20"<?php if($page_len == 20) { ?> selected="selected"<?php } ?>>20
              <option value="30"<?php if($page_len == 30) { ?> selected="selected"<?php } ?>>30
              <option value="50"<?php if($page_len == 50) { ?> selected="selected"<?php } ?>>50
           </select>
        </div>
    </div>
    <div class="control-group">
        <label class="control-label" for="theme"><?php print $text_theme; ?></label>
        <div class="controls">
               <select name="theme">
            <?php foreach(Registry::get('themes') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if(isset($_SESSION['theme']) && $_SESSION['theme'] == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
               </select>
        </div>
    </div>
    <div class="control-group">
        <label class="control-label" for="theme"><?php print $text_language; ?></label>
        <div class="controls">
               <select name="lang">
                  <option value=""><?php print $text_use_browser_settings; ?></option>
            <?php foreach(Registry::get('langs') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if(isset($_SESSION['lang']) && $_SESSION['lang'] == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
               </select>
        </div>
    </div>


    <div class="control-group">
        <div class="controls">
            <input type="submit" value="<?php print $text_set; ?>" class="btn btn-primary" /> <input type="reset" value="<?php print $text_cancel; ?>" class="btn btn" onclick="Piler.go_to_default_page();" />
        </div>
    </div>
    
</form>


<form method="post" name="setqr" class="form-horizontal">

   <h4><?php print $text_google_authenticator_settings; ?></h4>

    <div class="control-group">
        <label class="control-label" for="ga_enabled"><?php print $text_enable; ?></label>
        <div class="controls">
           <input type="checkbox" name="ga_enabled" id="ga_enabled" onclick="Piler.toggle_ga();" <?php if($ga['ga_enabled'] == 1) { ?>checked="checked"<?php } ?> />
        </div>
    </div>

    <div class="control-group">
        <label class="control-label" for="ga_secret"><?php print $text_qr_code; ?></label>
        <div id="QR" class="controls">
           <?php print $ga['ga_secret']; ?> <a href="#" onclick="Piler.new_qr(); return false;"> <?php print $text_refresh_qr_code; ?></a><br /><img src="qr.php?ts=<?php print time(); ?>" />
        </div>
    </div>

</form>



<p>&nbsp;</p>

<?php if(PASSWORD_CHANGE_ENABLED == 1) { ?>
<h4><?php print $text_change_password; ?></h4>
<form name="pwdchange" action="index.php?route=common/home" method="post" autocomplete="off">
   <table border="0" cellpadding="0" cellspacing="0">
      <tr><td><?php print $text_password; ?>: </td><td><input type="password" name="password" /></td></tr>
      <tr><td><?php print $text_password_again; ?>: </td><td><input type="password" name="password2" /></td></tr>
     <tr><td>&nbsp;</td><td><input type="submit" value="<?php print $text_submit; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></td></tr>
   </table>
</form>
<?php } ?>

<?php } else { ?>
<?php print $x; ?>. <a href="index.php?route=common/home"><?php print $text_back; ?></a>
<?php } ?>


