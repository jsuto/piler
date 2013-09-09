<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_import_job_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=import/remove&amp;id=-1&amp;name=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<h4><?php if(isset($id) && ($id > 0)) { print $text_edit_entry; } else { print $text_add_new_entry; } ?></h4>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=import/list" class="form-horizontal">

 <?php if(isset($id) && ($id > 0)) { ?>
    <input type="hidden" name="id" id="id" value="<?php print $id; ?>" />
 <?php } ?>

    <div class="control-group">
       <label class="control-label" for="type"><?php print $text_type; ?>:</label>
       <div class="controls">
          <select name="type" id="type">
                 <option value="pop3"<?php if(isset($a['type']) && $a['type'] == 'pop3') { ?> selected="selected"<?php } ?>>pop3</option>
                 <option value="imap"<?php if(isset($a['type']) && $a['type'] == 'imap') { ?> selected="selected"<?php } ?>>imap</option>
          </select>
       </div>
    </div>

    <div class="control-group<?php if(isset($errors['server'])){ print " error"; } ?>">
       <label class="control-label" for="server"><?php print $text_server_name; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="server" id="server" placeholder="" value="<?php if(isset($a['server'])) { print $a['server']; } ?>" />
          <?php if ( isset($errors['server']) ) { ?><span class="help-inline"><?php print $errors['server']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['username'])){ print " error"; } ?>">
		<label class="control-label" for="username"><?php print $text_username; ?>:</label>
        <div class="controls">
            <input type="text" class="text" name="username" id="username" placeholder="" value="<?php if(isset($a['username'])) { print $a['username']; } ?>" />
            <?php if ( isset($errors['username']) ) { ?><span class="help-inline"><?php print $errors['username']; ?></span><?php } ?>
        </div>
    </div>
    <div class="control-group<?php if(isset($errors['password'])){ print " error"; } ?>">
       <label class="control-label" for="password"><?php print $text_password; ?>:</label>
       <div class="controls">
          <input type="password" class="password" name="password" id="password" placeholder="" value="<?php if(isset($a['password'])) { print $a['password']; } ?>" /> <input type="button" value="<?php print $text_test_connection; ?>" class="btn btn-danger" onclick="Piler.test_pop3_connection(); return false;" /> <span id="LDAPTEST"></span>
          <?php if ( isset($errors['password']) ) { ?><span class="help-inline"><?php print $errors['password']; ?></span><?php } ?>
       </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php if(isset($id) && ($id > 0)) { print $text_modify; } else { print $text_add; } ?>" class="btn btn-primary" />
        <?php if(isset($id) && ($id > 0)) { ?>
            <a href="index.php?route=import/list" class="btn"><?php print $text_cancel; ?></a>
        <?php } else { ?>
            <input type="reset" value="<?php print $text_clear; ?>" class="btn" onclick="Piler.clear_ldap_test();" />
        <?php } ?>
    </div>

</form>

<?php if($id == -1) { ?>

<h4><?php print $text_existing_entries; ?> <a href="index.php?route=import/jobs"><?php print $text_view_progress; ?></a></h4>

<div class="listarea">

<?php if(isset($entries)){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th><?php print $text_type; ?></th>
         <th><?php print $text_username; ?></th>
         <th><?php print $text_password; ?></th>
         <th><?php print $text_server_name; ?></td>
         <th>&nbsp;</th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($entries as $e) { ?>
      <tr>
         <td><?php print $e['type']; ?></td>
         <td><?php print $e['username']; ?></td>
         <td>*******</td>
         <td><?php print $e['server']; ?></td>
         <td><a href="index.php?route=import/list&amp;id=<?php print $e['id']; ?>"><i class="icon-edit"></i>&nbsp;<?php print $text_edit; ?></a></td>
         <td><a href="index.php?route=import/remove&amp;id=<?php print $e['id']; ?>&amp;name=<?php print urlencode($e['username']); ?>&amp;confirmed=1" class="confirm-delete" data-id="<?php print $e['id']; ?>" data-name="<?php print $e['username']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
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

