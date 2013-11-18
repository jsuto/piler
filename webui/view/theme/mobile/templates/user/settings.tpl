
<?php if(!isset($x)){ ?>

<form action="settings.php" method="post" name="setpagelen" class="formbottom">

	<h4><?php print $text_access_settings; ?></h4>

   <div id="search">

	 <div class="row">
		<div class="logincell"><?php print $text_email_addresses; ?>:</div>
		<div class="logincell">
			<?php print $emails; ?>
		</div>
	 </div>   
   
<?php if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) { ?>
   
	 <div class="row">
		<div class="logincell"><?php print $text_domains; ?>:</div>
		<div class="logincell">
			<?php print $domains; ?>
		</div>
	 </div>

<?php } ?>
	 
<?php if(Registry::get('auditor_user') == 0 || RESTRICTED_AUDITOR == 0) { ?>
	 
	 <div class="row">
		<div class="logincell"><?php print $text_groups; ?>:</div>
		<div class="logincell">
			<?php print $groups; ?>
		</div>
	 </div>

<?php } ?>
	 
<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
	 
	 <div class="row">
		<div class="logincell"><?php print $text_folders; ?>:</div>
		<div class="logincell">
			<?php print $folders; ?>
		</div>
	 </div>

<?php } ?>
	 
	 <p><em><?php print $text_access_setting_explanation; ?></em></p>
	 
   </div>

   <div id="search">

	  <h4><?php print $text_display_settings; ?></h4>

      <div id="search1">

         <div class="row">
            <div class="logincell"><?php print $text_page_length; ?>:</div>
            <div class="tcell">
               <select name="pagelen">
               <?php foreach(Registry::get('paging') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if($page_len == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
               <?php } ?>
               </select>
            </div>
         </div>

         <div class="row">
            <div class="logincell"><?php print $text_theme; ?>:</div>
            <div class="tcell">
               <select name="theme">
            <?php foreach(Registry::get('themes') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if($theme == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
               </select>
            </div>
         </div>

         <div class="row">
            <div class="logincell"><?php print $text_language; ?>:</div>
            <div class="tcell">
               <select name="lang">
                  <option value=""><?php print $text_use_browser_settings; ?></option>
            <?php foreach(Registry::get('langs') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if($lang == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
               </select>
            </div>
         </div>


         <div class="row">
            <div class="logincell">&nbsp;</div>
            <div class="tcell"><input type="submit" value="<?php print $text_set; ?>" class="btn btn-primary" /> <input type="reset" class="btn" value="<?php print $text_cancel; ?>" onclick="Piler.go_to_default_page();" /></div>
         </div>


      </div>

   </div>
</form>

<p>&nbsp;</p>



   <div id="search">

      <h4><?php print $text_google_authenticator_settings; ?></h4>

      <div id="search1">

         <div class="row">
                <div class="logincell"><?php print $text_enable; ?>:</div>
                <div class="logincell"><input type="checkbox" name="ga_enabled" id="ga_enabled" onclick="Piler.toggle_ga();" <?php if($ga['ga_enabled'] == 1) { ?>checked="checked"<?php } ?> /></div>
         </div>

         <div class="row">
                <div class="logincell"><?php print $text_qr_code; ?>:</div>
                <div id="QR" class="logincell">
                   <?php print $ga['ga_secret']; ?> <a href="#" onclick="Piler.new_qr(); return false;"><?php print $text_refresh_qr_code; ?></a><br /><img src="qr.php?ts=<?php print time(); ?>" />

                </div>
         </div>
   </div>



<p>&nbsp;</p>



<?php if(PASSWORD_CHANGE_ENABLED == 1) { ?>
   <div id="search">
      <form method="post" name="pwdchange" action="settings.php" class="formbottom">
      <div id="ss1">
         <div class="row">
            <div class="domaincell"><?php print $text_password; ?>:</div>
            <div class="domaincell"><input type="password" class="text" name="password" /></div>
         </div>
         <div class="row">
            <div class="domaincell"><?php print $text_password_again; ?>:</div>
            <div class="domaincell"><input type="password" class="text" name="password2" /></div>
         </div>
         <div class="row">
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell"><input type="submit" class="btn btn-primary" value="<?php print $text_submit; ?>" /> <input type="reset" class="btn" value="<?php print $text_cancel; ?>" /></div>
         </div>
      </div>
   </form>
   </div>
<?php } ?>


<p>&nbsp;</p>

<?php } else { ?>
<?php print $x; ?>. <a href="settings.php"><?php print $text_back; ?></a>
<?php } ?>


