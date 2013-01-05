
<?php if(!isset($x)){ ?>

<form action="settings.php" method="post" name="setpagelen">

	<h4><?php print $text_access_settings; ?></h4>

   <div id="search">

	 <div class="row">
		<div class="cell1" style="width: 100px;"><?php print $text_email_addresses; ?>:</div>
		<div class="cell2">
			<?php print $emails; ?>
		</div>
	 </div>   
   
<?php if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) { ?>
   
	 <div class="row">
		<div class="cell1" style="width: 100px;"><?php print $text_domains; ?>:</div>
		<div class="cell2">
			<?php print $domains; ?>
		</div>
	 </div>

<?php } ?>
	 
<?php if(Registry::get('auditor_user') == 0 || RESTRICTED_AUDITOR == 0) { ?>
	 
	 <div class="row">
		<div class="cell1" style="width: 100px;"><?php print $text_groups; ?>:</div>
		<div class="cell2">
			<?php print $groups; ?>
		</div>
	 </div>

<?php } ?>
	 
<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
	 
	 <div class="row">
		<div class="cell1" style="width: 100px;"><?php print $text_folders; ?>:</div>
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
            <div class="cell1" style="width: 100px;"><?php print $text_page_length; ?>:</div>
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
            <div class="cell1">&nbsp;</div>
            <div class="cell2"><input type="submit" value="<?php print $text_set; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></div>
         </div>


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


