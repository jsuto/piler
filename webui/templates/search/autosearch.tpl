<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">

      <h4><?php print $text_add_new_entry; ?></h4>

      <form method="post" name="add1" action="index.php?route=search/autosearch">
      <div class="row g-3 align-items-center">
        <div class="col-auto">
          <label for="query" class="col-form-label"><?php print $text_search; ?></label>
        </div>
        <div class="col-auto">
          <input type="text" name="query" class="form-control" aria-describedby="emailHelpInline">
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

      <h4 class="mt-5"><?php print $text_existing_entries; ?></h4>

      <table class="table table-striped">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_search; ?></th>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($data as $d) { ?>
          <tr>
            <td><?php print $d['query']; ?></a></td>
            <td><a href="<?php print PATH_PREFIX; ?>index.php?route=search/autosearch&amp;remove=1&amp;id=<?php print $d['id']; ?>"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></i></a></td>
          </tr>
        <?php } ?>
        </tbody>
      </table>
    </div>
  </div>
</div>
