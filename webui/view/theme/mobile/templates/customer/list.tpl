<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_customer_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=customer/remove&amp;id=-1&amp;name=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

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
       <label class="control-label" for="colour"><?php print $text_background_colour; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="background_colour" id="background_colour" placeholder="" value="<?php if(isset($a['background_colour'])) { print $a['background_colour']; } ?>" oninput="Piler.change_box_colour('background_colour', 'cp');" /> <span id="cp" style="<?php if(isset($a['background_colour'])) { ?>background: <?php print $a['background_colour']; ?>;<?php } ?>">&nbsp;&nbsp;&nbsp;</span>
       </div>
    </div>
    <div class="control-group">
       <label class="control-label" for="colour"><?php print $text_text_colour; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="text_colour" id="text_colour" placeholder="" value="<?php if(isset($a['text_colour'])) { print $a['text_colour']; } ?>" oninput="Piler.change_box_colour('text_colour', 'cp2');" /> <span id="cp2" style="<?php if(isset($a['text_colour'])) { ?>background: <?php print $a['text_colour']; ?>;<?php } ?>">&nbsp;&nbsp;&nbsp;</span>
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

   <id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_domain; ?></div>
         <div class="domaincell"><?php print $text_branding_text; ?></div>
         <div class="domaincell"><?php print $text_branding_url; ?></div>
         <div class="domaincell"><?php print $text_branding_logo; ?></div>
         <div class="domaincell"><?php print $text_background_colour; ?></div>
         <div class="domaincell"><?php print $text_text_colour; ?></div>
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($entries as $e) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $e['domain']; ?></div>
         <div class="domaincell"><?php print $e['branding_text']; ?></div>
         <div class="domaincell"><?php print $e['branding_url']; ?></div>
         <div class="domaincell"><?php if($e['branding_logo']) { ?><img src="/images/<?php print $e['branding_logo']; ?>" /><?php } ?></div>
         <div class="domaincell"><?php print $e['background_colour']; ?></div>
         <div class="domaincell"><?php print $e['text_colour']; ?></div>
         <div class="domaincell"><a href="index.php?route=customer/list&amp;id=<?php print $e['id']; ?>"><?php print $text_edit; ?></a></div>
         <div class="domaincell"><a href="index.php?route=customer/remove&amp;id=<?php print $e['id']; ?>&amp;name=<?php print urlencode($e['domain']); ?>&amp;confirmed=1" class="confirm-delete" data-id="<?php print $e['id']; ?>" data-name="<?php print $e['domain']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></div>
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

