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

<form method="post" name="add1" action="index.php?route=policy/legalhold" class="formbottom">

   <div id="ss1">
      <div class="row">
         <div class="domaincell"><?php print $text_email; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="email" /></div>
      </div>

      <div class="row">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" class="btn btn-primary" value="<?php print $text_add; ?>" /> <input type="reset" class="btn" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>

</form>

<h4><?php print $text_existing_email; ?></h4>

<div class="listarea">

<?php if(count($emails) > 0){ ?>

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_email; ?></div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($emails as $email) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $email['email']; ?></div>
         <div class="domaincell"><a href="index.php?route=policy/removehold&amp;id=1&amp;email=<?php print urlencode($email['email']); ?>" class="confirm-delete" data-id="1" data-name="<?php print $email['email']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></div>
      </div>
<?php } ?>

   </div>

<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>

</div>

