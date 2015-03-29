<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_user_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=policy/removehold&amp;id=-1&amp;email=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<form method="get" name="search1" action="legalhold.php" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<h4><?php print $text_add_new_entry; ?></h4>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=policy/legalhold" class="form-horizontal">

    <div class="control-group<?php if(isset($errors['email'])){ print " error"; } ?>">
		<label class="control-label" for="email"><?php print $text_email; ?>:</label>
        <div class="controls">
            <input type="text" name="email" placeholder="Email"<?php if(isset($post['email'])){ echo " value='".$post['email']."'"; } ?> />
            <?php if ( isset($errors['email']) ) { ?><span class="help-inline"><?php print $errors['email']; ?></span><?php } ?>
        </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php print $text_add; ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" />
    </div>

</form>

<h4><?php print $text_existing_email; ?></h4>

<div class="listarea">

<?php if(isset($emails) && count($emails) > 0){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th><?php print $text_email; ?></th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($emails as $email) { ?>
      <tr>
         <td><?php print $email['email']; ?></a></td>
         <td><a href="index.php?route=policy/removehold&amp;id=1&amp;email=<?php print urlencode($email['email']); ?>" class="confirm-delete" data-id="1" data-name="<?php print $email['email']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
      </tr>
<?php } ?>

   </div>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

</div>

