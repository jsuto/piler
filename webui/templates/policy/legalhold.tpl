<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

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

<div class="container text-start">
  <div class="row">
    <div class="col">

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
            <button type="submit" class="btn btn-primary"><?php print $text_add; ?></button>
          </span>
        </div>
      </div>
      </form>
    </div>
  </div>
</div>


<div class="container text-start">
  <div class="row">
    <div class="col">

      <h4 class="mt-5"><?php print $text_existing_email; ?></h4>

      <table class="table table-striped">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_email; ?></th>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($emails as $d) { ?>
          <tr>
            <td><?php print $d['email']; ?></a></td>
            <td><a href="index.php?route=policy/removehold&amp;confirmed=1&amp;email=<?php print urlencode($d['email']); ?>"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></i></a></td>
          </tr>
        <?php } ?>
        </tbody>
      </table>

    </div>
  </div>
</div>
