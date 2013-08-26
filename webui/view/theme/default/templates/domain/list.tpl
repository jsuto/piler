<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_domain_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=domain/remove&amp;id=-1&amp;domain=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<form method="get" name="search1" action="domain.php" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<h4><?php print $text_add_new_domain; ?></h4>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=domain/domain" class="form-horizontal">

    <div class="control-group<?php if(isset($errors['domain'])){ print " error"; } ?>">
		<label class="control-label" for="domain"><?php print $text_domain; ?>:</label>
        <div class="controls">
            <input type="text" class="text" name="domain" placeholder="Domain"<?php if(isset($post['domain'])){ echo " value='".$post['domain']."'"; } ?> />
            <?php if ( isset($errors['domain']) ) { ?><span class="help-inline"><?php print $errors['domain']; ?></span><?php } ?>
        </div>
    </div>
    <div class="control-group<?php if(isset($errors['mapped'])){ print " error"; } ?>">
       <label class="control-label" for="mapped"><?php print $text_mapped_domain; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="mapped" placeholder="Mapped Domain"<?php if(isset($post['mapped'])){ echo " value='".$post['mapped']."'"; } ?> />
          <?php if ( isset($errors['mapped']) ) { ?><span class="help-inline"><?php print $errors['mapped']; ?></span><?php } ?>
       </div>
    </div>

<?php if(ENABLE_SAAS == 1) { ?>
    <div class="control-group">
       <label class="control-label" for="ldap_id"><?php print $text_ldap; ?>:</label>
       <div class="controls">
<select name="ldap_id" id="ldap_id">
          <option value="0"></option>
       <?php foreach ($ldap as $l) { ?>
          <option value="<?php print $l['id']; ?>"><?php print $l['description']; ?></option>
       <?php } ?>
</select>
       </div>
    </div>
<?php } ?>

    <div class="form-actions">
        <input type="submit" value="<?php print $text_add; ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" />
    </div>

</form>

<h4><?php print $text_existing_domains; ?></h4>

<div class="listarea">

<?php if(isset($domains) && count($domains) > 0){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th><?php print $text_domain; ?></th>
         <th><?php print $text_mapped_domain; ?></th>
      <?php if(ENABLE_SAAS == 1) { ?>
         <th><?php print $text_ldap; ?></th>
      <?php } ?>
         <th>&nbsp;</th>
      </tr>

<?php foreach($domains as $domain) { ?>
      <tr>
         <td><a href="index.php?route=user/list&search=@<?php print $domain['domain']; ?>"><?php print $domain['domain']; ?></a></td>
         <td><?php print $domain['mapped']; ?></td>
      <?php if(ENABLE_SAAS == 1) { ?>
         <td><?php if(isset($domain['ldap'])) {print $domain['ldap'];} else {print '&nbsp;';} ?></td>
      <?php } ?>
         <td><a href="index.php?route=domain/remove&amp;id=1&amp;domain=<?php print urlencode($domain['domain']); ?>" class="confirm-delete" data-id="1" data-name="<?php print urlencode($domain['domain']); ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
      </tr>
<?php } ?>

   </div>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

</div>

