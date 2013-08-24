<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_ldap_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=ldap/remove&amp;id=-1&amp;name=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<form method="get" name="search1" action="ldap.php" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<h4><?php if(isset($a['description'])) { print $text_edit_entry; } else { print $text_add_new_entry; } ?></h4>

<?php if(isset($x)){ ?>
    <div class="alert alert-info"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=ldap/list" class="form-horizontal">

 <?php if(isset($a['description'])) { ?>
    <input type="hidden" name="id" id="id" value="<?php print $id; ?>" />
 <?php } ?>

    <div class="control-group">
       <label class="control-label" for="ldap_type"><?php print $text_ldap_type; ?>:</label>
       <div class="controls">
          <select name="ldap_type" id="ldap_type">
       <?php while(list($k, $v) = each($ldap_types)) { ?>
          <option value="<?php print $v; ?>"<?php if(isset($a['ldap_type']) && $a['ldap_type'] == $v) { ?> selected="selected"<?php } ?>><?php print $v; ?></option>
       <?php } ?>
          </select>
       </div>
    </div>
    <div class="control-group">
		<label class="control-label" for="description"><?php print $text_description; ?>:</label>
        <div class="controls">
            <input type="text" class="text" name="description" id="description" placeholder="" value="<?php if(isset($a['description'])) { print $a['description']; } ?>" />
        </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_host"><?php print $text_ldap_host; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_host" id="ldap_host" placeholder="" value="<?php if(isset($a['ldap_host'])) { print $a['ldap_host']; } ?>" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_base_dn"><?php print $text_ldap_base_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_base_dn" id="ldap_base_dn" placeholder="" value="<?php if(isset($a['ldap_base_dn'])) { print $a['ldap_base_dn']; } ?>" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_bind_dn"><?php print $text_ldap_bind_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_bind_dn" id="ldap_bind_dn" placeholder="" value="<?php if(isset($a['ldap_bind_dn'])) { print $a['ldap_bind_dn']; } ?>" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_bind_pw"><?php print $text_ldap_bind_pw; ?>:</label>
       <div class="controls">
          <input type="password" class="password" name="ldap_bind_pw" id="ldap_bind_pw" placeholder="" value="<?php if(isset($a['ldap_bind_pw'])) { print $a['ldap_bind_pw']; } ?>" /> <input type="button" value="<?php print $text_test_connection; ?>" class="btn btn-danger" onclick="Piler.test_ldap_connection(); return false;" /> <span id="LDAPTEST"></span>
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_auditor_member_dn"><?php print $text_ldap_auditor_member_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_auditor_member_dn" id="ldap_auditor_member_dn" placeholder="" value="<?php if(isset($a['ldap_auditor_member_dn'])) { print $a['ldap_auditor_member_dn']; } ?>" />
       </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php if(isset($a['description'])) { print $text_modify; } else { print $text_add; } ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" onclick="Piler.clear_ldap_test();" />
    </div>

</form>

<?php if($id == -1) { ?>

<h4><?php print $text_existing_entries; ?></h4>

<div class="listarea">

<?php if(isset($entries)){ ?>

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_description; ?></div>
         <div class="domaincell"><?php print $text_ldap_type; ?></div>
         <div class="domaincell"><?php print $text_ldap_host; ?></div>
         <div class="domaincell"><?php print $text_ldap_base_dn; ?></div>
         <div class="domaincell"><?php print $text_ldap_bind_dn; ?></div>
         <div class="domaincell"><?php print $text_ldap_auditor_member_dn; ?></div>
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($entries as $e) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $e['description']; ?></div>
         <div class="domaincell"><?php print $e['ldap_type']; ?></div>
         <div class="domaincell"><?php print $e['ldap_host']; ?></div>
         <div class="domaincell"><?php print $e['ldap_base_dn']; ?></div>
         <div class="domaincell"><?php print $e['ldap_bind_dn']; ?></div>
         <div class="domaincell"><?php print $e['ldap_auditor_member_dn']; ?></div>
         <div class="domaincell"><a href="index.php?route=ldap/list&amp;id=<?php print $e['id']; ?>"><?php print $text_edit; ?></a></div>
         <div class="domaincell"><a href="index.php?route=ldap/remove&amp;id=<?php print $e['id']; ?>&amp;name=<?php print urlencode($e['description']); ?>&amp;confirmed=1" class="confirm-delete" data-id="<?php print $e['id']; ?>" data-name="<?php print $e['description']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></div>
      </div>
<?php } ?>

   </div>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

<?php } ?>


</div>

