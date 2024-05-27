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
    <a href="<?php print PATH_PREFIX; ?>index.php?route=customer/remove&amp;id=-1&amp;name=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<form method="get" name="search1" action="customer.php" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<h4><?php if(isset($id) && ($id > 0)) { print $text_edit_entry; } else { print $text_add_new_entry; } ?></h4>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="<?php print PATH_PREFIX; ?>index.php?route=customer/list" class="form-horizontal" enctype="multipart/form-data">

 <?php if(isset($id) && ($id > 0)) { ?>
    <input type="hidden" name="id" id="id" value="<?php print $id; ?>" />
 <?php } ?>

    <div class="control-group<?php if(isset($errors['domain'])){ print " error"; } ?>">
       <label class="control-label" for="domain"><?php print $text_domain; ?>:</label>
       <div class="controls">
          <select name="domain" id="domain">
       <?php foreach ($domains as $domain) { ?>
          <option value="<?php print $domain; ?>"<?php if(isset($a['domain']) && $a['domain'] == $domain) { ?> selected="selected"<?php } ?>><?php print $domain; ?></option>
       <?php } ?>
          </select>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['branding_text'])){ print " error"; } ?>">
       <label class="control-label" for="branding_text"><?php print $text_branding_text; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="branding_text" id="branding_text" placeholder="" value="<?php if(isset($a['branding_text'])) { print $a['branding_text']; } ?>" />
          <?php if ( isset($errors['branding_text']) ) { ?><span class="help-inline"><?php print $errors['branding_text']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['branding_url'])){ print " error"; } ?>">
       <label class="control-label" for="branding_url"><?php print $text_branding_url; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="branding_url" id="branding_url" placeholder="" value="<?php if(isset($a['branding_url'])) { print $a['branding_url']; } ?>" />
          <?php if ( isset($errors['branding_url']) ) { ?><span class="help-inline"><?php print $errors['branding_url']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['branding_logo'])){ print " error"; } ?>">
       <label class="control-label" for="branding_logo"><?php print $text_branding_logo; ?>:</label>
       <div class="controls">
          <div class="fileupload fileupload-new" data-provides="fileupload">
            <div class="fileupload-new thumbnail" style="width: 50px; height: 50px;">
                <?php if(isset($a['branding_logo'])) { ?><img src="/images/<?php print $a['branding_logo']; ?>" style="max-height: 50px;" /><?php } else { ?><img src="http://www.placehold.it/50x50/EFEFEF/AAAAAA" /><?php } ?>
            </div>
            <div class="fileupload-preview fileupload-exists thumbnail" style="width: 50px; height: 50px;"></div>
            <span class="btn btn-file"><span class="fileupload-new"><?php print $text_select_image; ?></span><span class="fileupload-exists"><?php print $text_modify; ?></span><input type="file" name="branding_logo" id="branding_logo" /></span>
            <a href="#" class="btn fileupload-exists" data-dismiss="fileupload"><?php print $text_remove; ?></a>
          </div>
          <?php if ( isset($errors['branding_logo']) ) { ?><span class="help-inline"><?php print $errors['branding_logo']; ?></span><?php } ?>
       </div>
    </div>
    <div class="control-group<?php if(isset($errors['support_link'])){ print " error"; } ?>">
       <label class="control-label" for="support_link"><?php print $text_support_link; ?>:</label>
       <div class="controls">
          <input type="text" class="text" name="support_link" id="support_link" placeholder="" value="<?php if(isset($a['support_link'])) { print $a['support_link']; } ?>" />
          <?php if ( isset($errors['support_link']) ) { ?><span class="help-inline"><?php print $errors['support_link']; ?></span><?php } ?>
       </div>
    </div>

    <div class="control-group<?php if(isset($errors['background_colour'])){ print " error"; } ?>">
       <label class="control-label" for="background_colour"><?php print $text_background_colour; ?>:</label>
       <div class="controls">
          <input type="text" class="text color {hash:true}" name="background_colour" id="background_colour" placeholder="" value="<?php if(isset($a['background_colour'])) { print $a['background_colour']; } ?>" />
          <?php if ( isset($errors['background_colour']) ) { ?><span class="help-inline"><?php print $errors['background_colour']; ?></span><?php } ?>
       </div>
    </div>

    <div class="control-group<?php if(isset($errors['text_colour'])){ print " error"; } ?>">
       <label class="control-label" for="text_colour"><?php print $text_text_colour; ?>:</label>
       <div class="controls">
          <input type="text" class="text color {hash:true}" name="text_colour" id="text_colour" placeholder="" value="<?php if(isset($a['text_colour'])) { print $a['text_colour']; } ?>" />
          <?php if ( isset($errors['text_colour']) ) { ?><span class="help-inline"><?php print $errors['text_colour']; ?></span><?php } ?>
       </div>
    </div>


    <div class="form-actions">
        <input type="submit" value="<?php if(isset($id) && ($id > 0)) { print $text_modify; } else { print $text_add; } ?>" class="btn btn-primary" />
        <?php if(isset($id) && ($id > 0)) { ?>
            <a href="<?php print PATH_PREFIX; ?>index.php?route=customer/list" class="btn"><?php print $text_cancel; ?></a>
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
         <th><?php print $text_domain; ?></th>
         <th><?php print $text_branding_text; ?></th>
         <th><?php print $text_branding_url; ?></th>
         <th><?php print $text_branding_logo; ?></th>
         <th><?php print $text_background_colour; ?></th>
         <th><?php print $text_text_colour; ?></th>
         <th>&nbsp;</th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($entries as $e) { ?>
      <tr>
         <td><?php print $e['domain']; ?></td>
         <td><?php print $e['branding_text']; ?></td>
         <td><?php print $e['branding_url']; ?></td>
         <td><?php if($e['branding_logo']) { ?><img src="/images/<?php print $e['branding_logo']; ?>" style="height: 50px;" /><?php } ?></td>
         <td><span class="label" style="background-color:<?php print $e['background_colour']; ?>"><?php print $e['background_colour']; ?></span></td>
         <td><span class="label" style="background-color:<?php print $e['text_colour']; ?>"><?php print $e['text_colour']; ?></span></td>
         <td><a href="<?php print PATH_PREFIX; ?>index.php?route=customer/list&amp;id=<?php print $e['id']; ?>"><i class="icon-edit"></i>&nbsp;<?php print $text_edit; ?></a></td>
         <td><a href="<?php print PATH_PREFIX; ?>index.php?route=customer/remove&amp;id=<?php print $e['id']; ?>&amp;name=<?php print urlencode($e['domain']); ?>&amp;confirmed=1" class="confirm-delete" data-id="<?php print $e['id']; ?>" data-name="<?php print $e['domain']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
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
