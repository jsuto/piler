
<?php if(!isset($_SESSION['username'])){ ?>
<form name="login" action="login.php" method="post">
   <input type="hidden" name="relocation" value="<?php if(isset($_GET['route']) && !preg_match("/^login/", $_GET['route']) ) { if(isset($_SERVER['REDIRECT_URL'])) { print $_SERVER['REDIRECT_URL']; } else { print $_SERVER['QUERY_STRING']; } } ?>" />


   <div id="search">

      <div id="search1">

<?php if(ENABLE_GOOGLE_LOGIN == 1) { ?>
         <div class="row">
            <div class="logincell"><a href="<?php print $auth_url; ?>"><?php print $text_login_via_google; ?></a></div>
         </div>
<?php } else { ?>
         <div class="row">
            <div class="logincell"><input type="text" class="input-medium" name="username" placeholder="<?php print $text_email; ?>" /></div>
         </div>

         <div class="row">
            <div class="logincell"><input type="password" class="input-medium" name="password" placeholder="<?php print $text_password; ?>" /></div>
         </div>

         <div class="row">
            <div class="logincell"><input type="submit" class="btn btn-primary" value="<?php print $text_submit; ?>" /> <input type="reset" class="btn" value="<?php print $text_cancel; ?>" /></div>
         </div>
<?php } ?>



<?php if(isset($x)){ ?>
         <div class="row">
            <div class="logincell"><p class="text-error bold"><?php print $x; ?></p></div>
         </div>
<?php } ?>



      </div>


   </div>


</form>



<?php } ?>

