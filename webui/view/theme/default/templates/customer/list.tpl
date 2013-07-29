<h4><?php if(isset($a['domain'])) { print $text_edit_entry; } else { print $text_add_new_entry; } ?></h4>

<?php if(isset($x)){ ?>
    <div class="alert alert-info"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=customer/list" class="form-horizontal" enctype="multipart/form-data">

 <?php if(isset($a['domain'])) { ?>
    <input type="hidden" name="id" id="id" value="<?php print $id; ?>" />
 <?php } ?>

    <div class="control-group">
       <label class="control-label" for="domain"><?php print $text_domain; ?>:</label>
       <div class="controls">
          <select name="domain" id="domain">
       <?php foreach ($domains as $domain) { ?>
          <option value="<?php print $domain; ?>"<?php if(isset($a['domain']) && $a['domain'] == $domain) { ?> selected="selected"<?php } ?>><?php print $domain; ?></option>
       <?php } ?>
          </select>
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="branding_text"><?php print $text_branding_text; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="branding_text" id="branding_text" placeholder="" value="<?php if(isset($a['branding_text'])) { print $a['branding_text']; } ?>" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="branding_url"><?php print $text_branding_url; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="branding_url" id="branding_url" placeholder="" value="<?php if(isset($a['branding_url'])) { print $a['branding_url']; } ?>" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="branding_logo"><?php print $text_branding_logo; ?>:</label>
       <div class="controls">
          <input type="file" class="text" name="branding_logo" id="branding_logo" placeholder="" /> <?php if(isset($a['branding_logo'])) { ?><img src="/images/<?php print $a['branding_logo']; ?>" /><?php } ?>
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="support_link"><?php print $text_support_link; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="support_link" id="support_link" placeholder="" value="<?php if(isset($a['support_link'])) { print $a['support_link']; } ?>" />
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="colour"><?php print $text_colour; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="colour" id="colour" placeholder="" value="<?php if(isset($a['colour'])) { print $a['colour']; } ?>" /> <?php if(isset($a['colour'])) { ?><span style="background: <?php print $a['colour']; ?>;">&nbsp;&nbsp;&nbsp;</span><?php } ?>
       </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php if(isset($a['domain'])) { print $text_modify; } else { print $text_add; } ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" onclick="Piler.clear_ldap_test();" />
    </div>

</form>

<?php if($id == -1) { ?>

<h4><?php print $text_existing_entries; ?></h4>

<div class="listarea">

<?php if(isset($entries)){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th class="domaincell"><?php print $text_domain; ?></th>
         <th class="domaincell"><?php print $text_branding_text; ?></th>
         <th class="domaincell"><?php print $text_branding_url; ?></th>
         <th class="domaincell"><?php print $text_branding_logo; ?></th>
         <th class="domaincell"><?php print $text_colour; ?></th>
         <th class="domaincell">&nbsp;</th>
         <th class="domaincell">&nbsp;</th>
      </tr>

<?php foreach($entries as $e) { ?>
      <tr>
         <td class="domaincell"><?php print $e['domain']; ?></td>
         <td class="domaincell"><?php print $e['branding_text']; ?></td>
         <td class="domaincell"><?php print $e['branding_url']; ?></td>
         <td class="domaincell"><?php if($e['branding_logo']) { ?><img src="/images/<?php print $e['branding_logo']; ?>" /><?php } ?></td>
         <td class="domaincell"><?php print $e['colour']; ?></td>
         <td class="domaincell"><a href="index.php?route=customer/list&amp;id=<?php print $e['id']; ?>"><?php print $text_edit; ?></a></td>
         <td class="domaincell"><a href="index.php?route=customer/remove&amp;id=<?php print $e['id']; ?>&amp;domain=<?php print urlencode($e['domain']); ?>&amp;confirmed=1" onclick="if(confirm('<?php print $text_remove; ?>: ' + '\'<?php print $e['domain']; ?>\'')) return true; return false;"><?php print $text_remove; ?></a></td>
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

