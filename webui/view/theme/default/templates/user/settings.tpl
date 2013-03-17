
<?php if(!isset($x)){ ?>

<form action="settings.php" method="post" name="setpagelen" class="formbottom">

	<h4><?php print $text_access_settings; ?></h4>

   <div id="search">

	 <div class="row">
		<div class="cell1 100px"><?php print $text_email_addresses; ?>:</div>
		<div class="cell2">
			<?php print $emails; ?>
		</div>
	 </div>   
   
<?php if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) { ?>
   
	 <div class="row">
		<div class="cell1 100px"><?php print $text_domains; ?>:</div>
		<div class="cell2">
			<?php print $domains; ?>
		</div>
	 </div>

<?php } ?>
	 
<?php if(Registry::get('auditor_user') == 0 || RESTRICTED_AUDITOR == 0) { ?>
	 
	 <div class="row">
		<div class="cell1 100px"><?php print $text_groups; ?>:</div>
		<div class="cell2">
			<?php print $groups; ?>
		</div>
	 </div>

<?php } ?>
	 
<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
	 
	 <div class="row">
		<div class="cell1 100px"><?php print $text_folders; ?>:</div>
		<div class="cell2">
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
            <div class="cell1 100px"><?php print $text_page_length; ?>:</div>
            <div class="cell2">
               <select name="pagelen">
                  <option value="10"<?php if($page_len == 10) { ?> selected="selected"<?php } ?>>10
                  <option value="20"<?php if($page_len == 20) { ?> selected="selected"<?php } ?>>20
                  <option value="30"<?php if($page_len == 30) { ?> selected="selected"<?php } ?>>30
                  <option value="50"<?php if($page_len == 50) { ?> selected="selected"<?php } ?>>50
               </select>
            </div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_theme; ?>:</div>
            <div class="cell2">
               <select name="theme">
            <?php foreach(Registry::get('themes') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if(isset($_SESSION['theme']) && $_SESSION['theme'] == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
               </select>
            </div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_language; ?>:</div>
            <div class="cell2">
               <select name="lang">
                  <option value=""><?php print $text_use_browser_settings; ?></option>
            <?php foreach(Registry::get('langs') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if(isset($_SESSION['lang']) && $_SESSION['lang'] == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
               </select>
            </div>
         </div>


         <div class="row">
            <div class="cell1">&nbsp;</div>
            <div class="cell2"><input type="submit" value="<?php print $text_set; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></div>
         </div>


      </div>

   </div>

</form>


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
            <div class="domaincell"><input type="submit" value="<?php print $text_submit; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></div>
         </div>
      </div>
   </form>
   </div>
<?php } ?>


<p>&nbsp;</p>

<?php } else { ?>
<?php print $x; ?>. <a href="settings.php"><?php print $text_back; ?></a>
<?php } ?>


