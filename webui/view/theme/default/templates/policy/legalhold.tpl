<div class="container text-start">
  <div class="row">
    <div class="col">
    </div>
    <div class="col text-end">
      <form method="get" name="search1" action="legalhold.php">
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

<h4><?php print $text_add_new_entry; ?></h4>

      <form method="post" name="add1" action="index.php?route=policy/legalhold">
      <div class="row g-3 align-items-center">
        <div class="col-auto">
          <label for="inputEmail" class="col-form-label"><?php print $text_email; ?></label>
        </div>
        <div class="col-auto">
          <input type="text" name="email" class="form-control" aria-describedby="emailHelpInline">
        </div>
        <div class="col-auto">
          <span id="emailHelpInline" class="form-text">
            <button type="button" class="btn btn-primary"><?php print $text_add; ?></button>
          </span>
        </div>
      </div>
      </form>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>


<h4 class="mt-5"><?php print $text_existing_email; ?></h4>

<div class="listarea">

<?php if(isset($emails) && count($emails) > 0){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th><?php print $text_email; ?></th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($emails as $email) { ?>
      <tr>
         <td><?php print $email['email']; ?></a></td>
         <td><a href="index.php?route=policy/removehold&amp;email=<?php print urlencode($email['email']); ?>&amp;confirmed=1"><i class="bi bi-trash text-danger"></i></a></td>
      </tr>
<?php } ?>

   </div>

<?php } else { print $text_not_found; } ?>

</div>
