
<h4 class="mt-4"><?php print $text_access_settings; ?></h4>

<p><em><?php print $text_access_setting_explanation; ?></em></p>

<div class="container text-start">
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_email_addresses; ?></div>
    <div class="col-2"><?php print $emails; ?></div>
  </div>

<?php if(Registry::get('auditor_user') == 1 && RESTRICTED_AUDITOR == 1) { ?>
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_domains; ?></div>
    <div class="col-2"><?php print $domains; ?></div>
  </div>
<?php } ?>

<?php if($wildcard_domains && (Registry::get('auditor_user') == 0 || RESTRICTED_AUDITOR == 0)) { ?>
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_wildcard_domains; ?></div>
    <div class="col-2"><?php print $wildcard_domains; ?></div>
  </div>
<?php } ?>

  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_groups; ?></div>
    <div class="col-2"><?php print $groups; ?></div>
  </div>

<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_folders; ?></div>
    <div class="col-2"><?php print $folders; ?></div>
  </div>
<?php } ?>

</div>



<form action="/settings.php" method="post" name="setpagelen" class="form-horizontal">

<h4 class="mt-5"><?php print $text_display_settings; ?></h4>

<div class="container text-start">
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_page_length; ?></div>
    <div class="col-2">
      <select name="pagelen" class="form-control">
      <?php foreach(Registry::get('paging') as $t) { ?>
        <option value="<?php print $t; ?>"<?php if($page_len == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
      <?php } ?>
      </select>
    </div>
  </div>

  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_theme; ?></div>
    <div class="col-2">
      <select name="theme" class="form-control">
      <?php foreach(Registry::get('themes') as $t) { ?>
        <option value="<?php print $t; ?>"<?php if($theme == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
      <?php } ?>
      </select>
    </div>
  </div>

  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_language; ?></div>
    <div class="col-2">
      <select name="lang" class="form-control">
        <option value=""><?php print $text_use_browser_settings; ?></option>
      <?php foreach(Registry::get('langs') as $t) { ?>
        <option value="<?php print $t; ?>"<?php if($lang == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
      <?php } ?>
      </select>
    </div>
  </div>

  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_page_length; ?></div>
    <div class="col-2">
      <select name="pagelen" class="form-control">
      <?php foreach(Registry::get('paging') as $t) { ?>
        <option value="<?php print $t; ?>"<?php if($page_len == $t) { ?> selected="selected"<?php } ?>><?php print $t; ?></option>
      <?php } ?>
      </select>
    </div>
  </div>

  <div class="row justify-content-start mt-3">
    <div class="col-2"></div>
    <div class="col-2">
      <input type="submit" class="btn btn-primary" value="<?php print $text_submit; ?>" />
      <input type="reset" class="btn btn-secondary" value="<?php print $text_cancel; ?>" />
    </div>
  </div>
</div>

</form>



<?php if(ENABLE_GOOGLE_AUTHENTICATOR == 1) { ?>

<form method="post" name="setqr" class="form-horizontal">

<h4 class="mt-5"><?php print $text_google_authenticator_settings; ?></h4>

<div class="container text-start">
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_enable; ?></div>
    <div class="col-2">
      <input type="checkbox" name="ga_enabled" id="ga_enabled" onclick="Piler.toggle_ga();" <?php if($ga['ga_enabled'] == 1) { ?>checked="checked"<?php } ?> />
    </div>
  </div>
  <div class="row justify-content-start">
    <div class="col-2">
      <a href="#" onclick="Piler.new_qr(); return false;"> <?php print $text_refresh_qr_code; ?></a>
    </div>
    <div id="QR" class="col-2">
      <?php print $ga['ga_secret']; ?><br />
      <img src="qr.php?ts=<?php print time(); ?>" />
    </div>
  </div>
</div>


<?php } ?>


<?php if(PASSWORD_CHANGE_ENABLED == 1) { ?>
<h4 class="mt-5"><?php print $text_change_password; ?></h4>

<form name="pwdchange" action="/settings.php" method="post" autocomplete="off">

<div class="container text-start">
  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_password; ?></div>
    <div class="col-2"><input type="password" id="password" class="form-control"></div>
  </div>

  <div class="row justify-content-start">
    <div class="col-2"><?php print $text_password_again; ?></div>
    <div class="col-2"><input type="password" id="password2" class="form-control"></div>
  </div>

  <div class="row justify-content-start mt-3">
    <div class="col-2"></div>
    <div class="col-2">
      <input type="submit" class="btn btn-primary" value="<?php print $text_submit; ?>" />
      <input type="reset" class="btn btn-secondary" value="<?php print $text_cancel; ?>" />
    </div>
  </div>

</div>

</form>
<?php } ?>
