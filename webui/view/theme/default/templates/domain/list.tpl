<h4><?php print $text_add_new_domain; ?></h4>

<?php if(isset($x)){ ?>
    <div class="alert alert-info"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=domain/domain" class="form-horizontal">

    <div class="control-group">
		<label class="control-label" for="domain"><?php print $text_domain; ?>:</label>
        <div class="controls">
            <input type="text" class="text" name="domain" placeholder="Domain" />
        </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="mapped"><?php print $text_mapped_domain; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="mapped" placeholder="Mapped Domain" />
       </div>
    </div>

<?php if(ENABLE_SAAS == 1) { ?>
    <div class="control-group">
       <label class="control-label" for="ldap_id"><?php print $text_ldap; ?>:</label>
       <div class="controls">
<select name="ldap_id" id="ldap_id">
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

<?php if(isset($domains)){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th class="domaincell"><?php print $text_domain; ?></th>
         <th class="domaincell"><?php print $text_mapped_domain; ?></th>
      <?php if(ENABLE_SAAS == 1) { ?>
         <th class="domaincell"><?php print $text_ldap; ?></th>
      <?php } ?>
         <th class="domaincell">&nbsp;</th>
      </tr>

<?php foreach($domains as $domain) { ?>
      <tr>
         <td class="domaincell"><a href="index.php?route=user/list&search=@<?php print $domain['domain']; ?>"><?php print $domain['domain']; ?></a></td>
         <td class="domaincell"><?php print $domain['mapped']; ?></td>
      <?php if(ENABLE_SAAS == 1) { ?>
         <td class="domaincell"><?php print $domain['ldap']; ?></td>
      <?php } ?>
         <td class="domaincell"><a href="index.php?route=domain/remove&amp;domain=<?php print urlencode($domain['domain']); ?>" onclick="javascript:confirm('Delete Domain','Delete','Cancel','<?php print urlencode($domain['domain']); ?>')"><?php print $text_remove; ?></a></td>
      </tr>
<?php } ?>

   </div>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

</div>

