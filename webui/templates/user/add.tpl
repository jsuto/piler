<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">

      <form method="post" name="add1" action="<?php print PATH_PREFIX; ?>index.php?route=user/add" autocomplete="off">
      <input type="hidden" name="uid" value="<?php print $next_user_id; ?>" />

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="email" class="col-form-label"><?php print $text_email_addresses; ?></label>
        </div>
        <div class="col-4">
          <textarea name="email" id="email" class="form-control" aria-describedby="help1"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text">The user's email addresses</span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="username" class="col-form-label"><?php print $text_username; ?></label>
        </div>
        <div class="col-4">
          <input type="text" name="username" id="username" class="form-control" value="<?php if(isset($post['username'])){ print $post['username']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="realname" class="col-form-label"><?php print $text_realname; ?></label>
        </div>
        <div class="col-4">
          <input type="text" name="realname" id="realname" class="form-control" value="<?php if(isset($post['realname'])){ print $post['realname']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="domain" class="col-form-label"><?php print $text_domain; ?></label>
        </div>
        <div class="col-4">
          <select name="domain" id="domain" class="form-control">
          <?php asort($domains); foreach ($domains as $domain) { ?>
            <option value="<?php if(is_array($domain)){ print $domain['mapped']; } else { print $domain; } ?>"<?php if( (isset($post) && $domain == $post['domain']) || (!isset($post) && $domain == $last_domain) ){ ?> selected="selected"<?php } ?>><?php if(is_array($domain)){ print $domain['mapped']; } else { print $domain; } ?></option>
          <?php } ?>
          </select>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>


    <?php if(ENABLE_LDAP_IMPORT_FEATURE == 1) { ?>
      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="dn" class="col-form-label">LDAP DN</label>
        </div>
        <div class="col-4">
          <input type="text" name="dn" id="dn" class="form-control" value="<?php if(isset($post['dn'])){ print $post['dn']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"><?php print $text_dn_asterisk_means_skip_sync; ?></span>
        </div>
      </div>
    <?php } ?>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="password" class="col-form-label"><?php print $text_password; ?></label>
        </div>
        <div class="col-4">
          <input type="password" name="password" id="password" class="form-control" value="<?php if(isset($post['password'])){ print $post['password']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="password2" class="col-form-label"><?php print $text_password_again; ?></label>
        </div>
        <div class="col-4">
          <input type="password" name="password2" id="password2" class="form-control" value="<?php if(isset($post['password2'])){ print $post['password2']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="isadmin" class="col-form-label"><?php print $text_admin_user; ?></label>
        </div>
        <div class="col-4">
          <select name="isadmin" id="isadmin" class="form-control">
            <option value="0"<?php if(isset($post['isadmin']) && $post['isadmin'] == 0){ ?> selected="selected"<?php } ?>><?php print $text_user_regular; ?></option>
          <?php if(Registry::get('admin_user') == 1) { ?>
            <option value="1"<?php if(isset($post['isadmin']) && $post['isadmin'] == 1){ ?> selected="selected"<?php } ?>><?php print $text_user_masteradmin; ?></option>
          <?php } ?>
            <option value="2"<?php if(isset($post['isadmin']) && $post['isadmin'] == 2){ ?> selected="selected"<?php } ?>><?php print $text_user_auditor; ?></option>
            <option value="4"<?php if(isset($post['isadmin']) && $post['isadmin'] == 4){ ?> selected="selected"<?php } ?>><?php print $text_user_data_officer; ?></option>
          </select>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center mt-3">
        <div class="col-2">
        </div>
        <div class="col-2">
          <button type="submit" class="btn btn-primary"><?php print $text_add; ?></button>
          <a href="<?php print PATH_PREFIX; ?>index.php?route=user/list" type="reset" class="btn btn-secondary"><?php print $text_cancel; ?></a>
        </div>
      </div>

      </form>

    </div>
  </div>
</div>
