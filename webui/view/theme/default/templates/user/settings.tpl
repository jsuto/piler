
<?php if(!isset($x)){ ?>

<form action="/settings.php" method="post" name="setpagelen">

   <div id="search">

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
            <div class="cell1"><?php print $text_language; ?>:</div>
            <div class="cell2">
               <select name="lang">
            <?php foreach(Registry::get('langs') as $t) { ?>
                  <option value="<?php print $t; ?>"<?php if(isset($_SESSION['lang']) && $_SESSION['lang'] == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
            <?php } ?>
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


