<h4><?php print $text_add_new_entry; ?></h4>

<?php if(isset($x)){ ?>
    <div class="alert alert-info"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=ldap/list" class="form-horizontal">

    <div class="control-group">
		<label class="control-label" for="description"><?php print $text_description; ?>:</label>
        <div class="controls">
            <input type="text" class="text" name="description" id="description" placeholder="" />
        </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_host"><?php print $text_ldap_host; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_host" id="ldap_host" placeholder="" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_base_dn"><?php print $text_ldap_base_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_base_dn" id="ldap_base_dn" placeholder="" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_bind_dn"><?php print $text_ldap_bind_dn; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="ldap_bind_dn" id="ldap_bind_dn" placeholder="" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="ldap_bind_pw"><?php print $text_ldap_bind_pw; ?>:</label>
       <div class="controls">
          <input type="password" class="password" name="ldap_bind_pw" id="ldap_bind_pw" placeholder="" /> <input type="button" value="<?php print $text_test_connection; ?>" class="btn btn-danger" onclick="Piler.test_ldap_connection(); return false;" /> <span id="LDAPTEST"></span>
       </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php print $text_add; ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" onclick="Piler.clear_ldap_test();" />
    </div>

</form>

<h4><?php print $text_existing_entries; ?></h4>

<div class="listarea">

<?php if(isset($entries)){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th class="domaincell"><?php print $text_description; ?></th>
         <th class="domaincell"><?php print $text_ldap_host; ?></th>
         <td class="domaincell"><?php print $text_ldap_base_dn; ?></td>
         <td class="domaincell"><?php print $text_ldap_bind_dn; ?></td>
         <th class="domaincell">&nbsp;</th>
      </tr>

<?php foreach($entries as $e) { ?>
      <tr>
         <td class="domaincell"><?php print $e['description']; ?></td>
         <td class="domaincell"><?php print $e['ldap_host']; ?></td>
         <td class="domaincell"><?php print $e['ldap_base_dn']; ?></td>
         <td class="domaincell"><?php print $e['ldap_bind_dn']; ?></td>
         <td class="domaincell"><a href="index.php?route=ldap/remove&amp;id=<?php print $e['id']; ?>&amp;description=<?php print urlencode($e['description']); ?>&amp;confirmed=1" onclick="if(confirm('<?php print $text_remove; ?>: ' + '\'<?php print $e['description']; ?>\'')) return true; return false;"><?php print $text_remove; ?></a></td>
      </tr>
<?php } ?>

   </div>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

</div>

