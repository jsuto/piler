<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">

      <form method="post" name="add1" action="<?php print PATH_PREFIX; ?>index.php?route=group/add">
      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="groupname" class="col-form-label"><?php print $text_groupname; ?></label>
        </div>
        <div class="col-4">
          <input type="text" name="groupname" id="groupname" class="form-control" value="<?php if(isset($post['groupname'])){ print $post['groupname']; } ?>" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="mapped" class="col-form-label"><?php print $text_email_addresses; ?></label>
        </div>
        <div class="col-4">
          <textarea name="email" class="form-control" aria-describedby="help2"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea>
        </div>
        <div class="col-auto">
          <span id="help2" class="form-text">Email addresses belonging to the group</span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="mapped" class="col-form-label"><?php print $text_assigned_email_addresses; ?></label>
        </div>
        <div class="col-4">
          <textarea name="assigned_email" class="form-control" aria-describedby="help3"><?php if(isset($post['assigned_email'])){ print $post['assigned_email']; } ?></textarea>
        </div>
        <div class="col-auto">
          <span id="help3" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center mt-3">
        <div class="col-2">
        </div>
        <div class="col-2">
          <button type="submit" class="btn btn-primary"><?php print $text_add; ?></button>
          <a href="<?php print PATH_PREFIX; ?>index.php?route=group/list" type="reset" class="btn btn-secondary"><?php print $text_cancel; ?></a>
        </div>
      </div>

      </form>
    </div>
  </div>
</div>
