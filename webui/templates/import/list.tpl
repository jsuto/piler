<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-success text-start">
<?php if(isset($x)) { print $x; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">


      <h4><?php if(isset($id) && ($id > 0)) { print $text_edit_entry; } else { print $text_add_new_entry; } ?></h4>

      <form method="post" name="add1" action="<?php print PATH_PREFIX; ?>index.php?route=import/list" class="form-horizontal">

      <?php if(isset($id) && ($id > 0)) { ?>
        <input type="hidden" name="id" id="id" value="<?php print $id; ?>" />
      <?php } ?>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="type" class="col-form-label"><?php print $text_type; ?></label>
        </div>
        <div class="col-4">
          <select name="type" id="type" class="form-control">
            <option value="pop3"<?php if(isset($a['type']) && $a['type'] == 'pop3') { ?> selected="selected"<?php } ?>>pop3</option>
            <option value="imap"<?php if(isset($a['type']) && $a['type'] == 'imap') { ?> selected="selected"<?php } ?>>imap</option>
            <option value="imap-ssl"<?php if(isset($a['type']) && $a['type'] == 'imap-ssl') { ?> selected="selected"<?php } ?>>imap-ssl</option>
            <option value="imap-tls"<?php if(isset($a['type']) && $a['type'] == 'imap-tls') { ?> selected="selected"<?php } ?>>imap-tls</option>
          </select>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text">Type of server</span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="server" class="col-form-label"><?php print $text_server_name; ?></label>
        </div>
        <div class="col-4">
          <input type="text" name="server" id="server" class="form-control" value="<?php if(isset($a['server'])){ print $a['server']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text">Hostname of the POP3 or IMAP server</span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="username" class="col-form-label"><?php print $text_username; ?></label>
        </div>
        <div class="col-4">
          <input type="text" name="username" id="username" class="form-control" value="<?php if(isset($a['username'])){ print $a['username']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="password" class="col-form-label"><?php print $text_password; ?></label>
        </div>
        <div class="col-4">
          <input type="password" name="password" id="password" class="form-control" value="<?php if(isset($a['password'])){ print $a['password']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text">
            <button class="btn btn-danger" onclick="Piler.test_pop3_connection(); return false;" /><?php print $text_test_connection; ?></button> <span id="LDAPTEST"></span>
          </span>
        </div>
      </div>

      <div class="row g-3 align-items-center mt-2">
        <div class="col-2">
        </div>
        <div class="col-2">
          <input type="submit" value="<?php if(isset($id) && ($id > 0)) { print $text_modify; } else { print $text_add; } ?>" class="btn btn-primary" />
        <?php if(isset($id) && ($id > 0)) { ?>
            <a href="<?php print PATH_PREFIX; ?>index.php?route=import/list" class="btn btn-secondary"><?php print $text_cancel; ?></a>
        <?php } else { ?>
            <button class="btn btn-secondary" onclick="Piler.clear_ldap_test();"><?php print $text_clear; ?></button>
        <?php } ?>
          </span>
        </div>
      </div>

      </form>
    </div>
  </div>
</div>


<div class="container text-start mt-3">
  <div class="row">
    <div class="col">


      <h4><?php print $text_existing_entries; ?> <a href="<?php print PATH_PREFIX; ?>index.php?route=import/jobs"><?php print $text_view_progress; ?></a></h4>

      <table class="table table-striped mt-3">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_type; ?></th>
            <th><?php print $text_username; ?></th>
            <th><?php print $text_password; ?></th>
            <th><?php print $text_server_name; ?></td>
            <th>&nbsp;</th>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($entries as $e) { ?>
          <tr>
            <td><?php print $e['type']; ?></td>
            <td><?php print $e['username']; ?></td>
            <td>*******</td>
            <td><?php print $e['server']; ?></td>
            <td><a href="<?php print PATH_PREFIX; ?>index.php?route=import/list&amp;id=<?php print $e['id']; ?>"><i class="bi bi-pencil-square" title="<?php print $text_edit; ?>"></i></a></td>
            <td><a href="<?php print PATH_PREFIX; ?>index.php?route=import/remove&amp;id=<?php print $e['id']; ?>&amp;confirmed=1"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></i></a></td>
         </tr>
       <?php } ?>
        </tbody>
      </table>
    </div>
  </div>
</div>
