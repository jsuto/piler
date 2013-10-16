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

<form method="post" name="add1" action="index.php?route=domain/domain" class="formbottom">

   <div id="ss1">
      <div class="row">
         <div class="domaincell"><?php print $text_domain; ?>:</div>
         <div class="domaincell"><textarea name="domain"></textarea></div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_mapped_domain; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="mapped" /></div>
      </div>
<?php if(ENABLE_SAAS == 1) { ?>
      <div class="row">
         <div class="domaincell"><?php print $text_ldap; ?>:</div>
         <div class="domaincell">
            <select name="ldap_id" id="ldap_id">
               <option value="0"></option>
       <?php foreach ($ldap as $l) { ?>
               <option value="<?php print $l['id']; ?>"><?php print $l['description']; ?></option>
       <?php } ?>
            </select>
       </div>
    </div>
<?php } ?>


      <div class="row">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" class="btn btn-primary" value="<?php print $text_add; ?>" /> <input type="reset" class="btn" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>

</form>

<h4><?php print $text_existing_domains; ?></h4>

<div class="listarea">

<?php if(isset($domains)){ ?>

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_domain; ?></div>
         <div class="domaincell"><?php print $text_mapped_domain; ?></div>
      <?php if(ENABLE_SAAS == 1) { ?>
         <div class="domaincell"><?php print $text_ldap; ?></div>
      <?php } ?>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($domains as $domain) { ?>
      <div class="domainrow">
         <div class="domaincell"><a href="index.php?route=user/list&search=@<?php print $domain['domain']; ?>"><?php print $domain['domain']; ?></a></div>
         <div class="domaincell"><?php print $domain['mapped']; ?></div>
      <?php if(ENABLE_SAAS == 1) { ?>
         <div class="domaincell"><?php print $domain['ldap']; ?></div>
      <?php } ?>
         <div class="domaincell"><a href="index.php?route=domain/remove&amp;id=1&amp;domain=<?php print urlencode($domain['domain']); ?>" class="confirm-delete" data-id="1" data-name="<?php print urlencode($domain['domain']); ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></div>
      </div>
<?php } ?>

   </div>

<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>

</div>

