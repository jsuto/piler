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

<h4><?php if(isset($id) && ($id > 0)) { print $text_edit_entry; } else { print $text_add_new_entry; } ?></h4>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=ldap/list" class="form-horizontal">

 <?php if(isset($id) && ($id > 0)) { ?>
    <input type="hidden" name="id" id="id" value="<?php print $id; ?>" />
 <?php } ?>

    <div class="control-group<?php if(isset($errors['ldap_type'])){ print " error"; } ?>">
       <label class="control-label" for="ldap_type"><?php print $text_ldap_type; ?>:</label>
       <div class="controls">
          <select name="ldap_type" id="ldap_type">
       <?php while(list($k, $v) = each($ldap_types)) { ?>
          <option value="<?php print $v; ?>"<?php if(isset($a['ldap_type']) && $a['ldap_type'] == $v) { ?> selected="selected"<?php } ?>><?php print $v; ?></option>
       <?php } ?>
          </select>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['description'])){ print " error"; } ?>">
		<label class="control-label" for="description"><?php print $text_description; ?>:</label>
        <div class="controls">
            <input type="text" class="text" name="description" id="description" placeholder="" value="<?php if(isset($a['description'])) { print $a['description']; } ?>" />
            <?php if ( isset($errors['description']) ) { ?><span class="help-inline"><?php print $errors['description']; ?></span><?php } ?>
        </div>
    </div>
    <div class="control-group<?php if(isset($errors['ldap_host'])){ print " error"; } ?>">
       <label class="control-label" for="ldap_host"><?php print $text_ldap_host; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_host" id="ldap_host" placeholder="" value="<?php if(isset($a['ldap_host'])) { print $a['ldap_host']; } ?>" />
          <?php if ( isset($errors['ldap_host']) ) { ?><span class="help-inline"><?php print $errors['ldap_host']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['ldap_base_dn'])){ print " error"; } ?>">
       <label class="control-label" for="ldap_base_dn"><?php print $text_ldap_base_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_base_dn" id="ldap_base_dn" placeholder="" value="<?php if(isset($a['ldap_base_dn'])) { print $a['ldap_base_dn']; } ?>" />
          <?php if ( isset($errors['ldap_base_dn']) ) { ?><span class="help-inline"><?php print $errors['ldap_base_dn']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['ldap_bind_dn'])){ print " error"; } ?>">
       <label class="control-label" for="ldap_bind_dn"><?php print $text_ldap_bind_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_bind_dn" id="ldap_bind_dn" placeholder="" value="<?php if(isset($a['ldap_bind_dn'])) { print $a['ldap_bind_dn']; } ?>" />
          <?php if ( isset($errors['ldap_bind_dn']) ) { ?><span class="help-inline"><?php print $errors['ldap_bind_dn']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['ldap_bind_pw'])){ print " error"; } ?>">
       <label class="control-label" for="ldap_bind_pw"><?php print $text_ldap_bind_pw; ?>:</label>
       <div class="controls">
          <input type="password" class="password" name="ldap_bind_pw" id="ldap_bind_pw" placeholder="" value="<?php if(isset($a['ldap_bind_pw'])) { print $a['ldap_bind_pw']; } ?>" /> <input type="button" value="<?php print $text_test_connection; ?>" class="btn btn-danger" onclick="Piler.test_ldap_connection(); return false;" /> <span id="LDAPTEST"></span>
          <?php if ( isset($errors['ldap_bind_pw']) ) { ?><span class="help-inline"><?php print $errors['ldap_bind_pw']; ?></span><?php } ?>
       </div>
    </div> 
    <div class="control-group<?php if(isset($errors['ldap_auditor_member_dn'])){ print " error"; } ?>">
       <label class="control-label" for="ldap_auditor_member_dn"><?php print $text_ldap_auditor_member_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_auditor_member_dn" id="ldap_auditor_member_dn" placeholder="" value="<?php if(isset($a['ldap_auditor_member_dn'])) { print $a['ldap_auditor_member_dn']; } ?>" />
          <?php if ( isset($errors['ldap_auditor_member_dn']) ) { ?><span class="help-inline"><?php print $errors['ldap_auditor_member_dn']; ?></span><?php } ?>
       </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php if(isset($id) && ($id > 0)) { print $text_modify; } else { print $text_add; } ?>" class="btn btn-primary" />
        <?php if(isset($id) && ($id > 0)) { ?>
            <a href="index.php?route=ldap/list" class="btn"><?php print $text_cancel; ?></a>
        <?php } else { ?>
            <input type="reset" value="<?php print $text_clear; ?>" class="btn" onclick="Piler.clear_ldap_test();" />
        <?php } ?>
    </div>

</form>

<?php if($id == -1) { ?>

<h4><?php print $text_existing_entries; ?></h4>

<div class="listarea">

<?php if(isset($entries)){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th><?php print $text_description; ?></th>
         <th><?php print $text_ldap_type; ?></th>
         <th><?php print $text_ldap_host; ?></th>
         <th><?php print $text_ldap_base_dn; ?></td>
         <th><?php print $text_ldap_bind_dn; ?></td>
         <th><?php print $text_ldap_auditor_member_dn; ?></td>
         <th>&nbsp;</th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($entries as $e) { ?>
      <tr>
         <td><?php print $e['description']; ?></td>
         <td><?php print $e['ldap_type']; ?></td>
         <td><?php print $e['ldap_host']; ?></td>
         <td><?php print $e['ldap_base_dn']; ?></td>
         <td><?php print $e['ldap_bind_dn']; ?></td>
         <td><?php print $e['ldap_auditor_member_dn']; ?></td>
         <td><a href="index.php?route=ldap/list&amp;id=<?php print $e['id']; ?>"><i class="icon-edit"></i>&nbsp;<?php print $text_edit; ?></a></td>
         <td><a href="index.php?route=ldap/remove&amp;id=<?php print $e['id']; ?>&amp;name=<?php print urlencode($e['description']); ?>&amp;confirmed=1" class="confirm-delete" data-id="<?php print $e['id']; ?>" data-name="<?php print $e['description']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
      </tr>
<?php } ?>

   </table>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

<?php } ?>


</div>

