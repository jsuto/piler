<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">
    </div>
    <div class="col text-end">
      <form method="get" name="search1" action="/domain.php">
        <div class="row justify-content-end">
          <div class="col-8">
            <input type="text" name="search" class="form-control" value="<?php print $search; ?>">
          </div>
          <div class="col-2">
            <button type="submit" class="btn btn-primary"><?php print $text_search; ?></button>
          </div>
        </div>
      </form>
    </div>
  </div>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">

      <h4><?php print $text_edit_group; ?></h4>

      <form method="post" name="add1" action="index.php?route=group/edit">
        <input type="hidden" name="id" value="<?php print $id; ?>" />

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="groupname" class="col-form-label"><?php print $text_groupname; ?></label>
        </div>
        <div class="col-4">
          <input type="text" name="groupname" id="groupname" class="form-control" value="<?php if(isset($group['groupname'])){ print $group['groupname']; } ?>" aria-describedby="help1" />
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
          <textarea name="email" class="form-control" aria-describedby="help2"><?php if(isset($email)){ print $email; } ?></textarea>
        </div>
        <div class="col-auto">
          <span id="help2" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="mapped" class="col-form-label"><?php print $text_assigned_email_addresses; ?></label>
        </div>
        <div class="col-4">
          <textarea name="assigned_email" class="form-control" aria-describedby="help3"><?php if(isset($assigned_email)){ print $assigned_email; } ?></textarea>
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
          <a href="index.php?route=group/list" type="reset" class="btn btn-secondary"><?php print $text_cancel; ?></a>
        </div>
      </div>

      </form>
    </div>
  </div>
</div>
