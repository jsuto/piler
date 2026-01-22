<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">
    </div>
    <div class="col text-end">
      <form method="get" name="search1" action="exclusion.php">
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

      <h4><?php print $text_add_new_rule; ?></h4>

      <form method="post" id="add1" name="add1" action="<?php print PATH_PREFIX; ?>index.php?route=policy/exclusion" class="form-horizontal">

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="from" class="col-form-label"><?php print $text_from; ?></label>
        </div>
        <div class="col-5">
          <input type="text" name="from" id="from" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="to" class="col-form-label"><?php print $text_to; ?>/<?php print $text_cc; ?></label>
        </div>
        <div class="col-5">
          <input type="text" name="to" id="to" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="subject" class="col-form-label"><?php print $text_subject; ?></label>
        </div>
        <div class="col-5">
          <input type="text" name="subject" id="subject" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="body" class="col-form-label"><?php print $text_body; ?></label>
        </div>
        <div class="col-5">
          <input type="text" name="body" id="body" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="size" class="col-form-label"><?php print $text_size; ?></label>
        </div>
        <div class="col-1">
          <select class="form-control" name="_size">
            <option value=">">&gt;</option>
            <option value="=">=</option>
            <option value="<">&lt;</option>
          </select>
        </div>
        <div class="col-4">
          <input type="text" name="size" id="size" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="attachment_name" class="col-form-label"><?php print $text_attachment_name; ?></label>
        </div>
        <div class="col-5">
          <input type="text" name="attachment_name" id="attachment_name" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="attachment_type" class="col-form-label"><?php print $text_attachment_type; ?></label>
        </div>
        <div class="col-5">
          <input type="text" name="attachment_type" id="attachment_type" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="attachment_size" class="col-form-label"><?php print $text_attachment_size; ?></label>
        </div>
        <div class="col-1">
          <select class="form-control" name="_attachment_size">
            <option value=">">&gt;</option>
            <option value="=">=</option>
            <option value="<">&lt;</option>
          </select>
        </div>
        <div class="col-4">
          <input type="text" name="attachment_size" id="attachment_size" class="form-control" aria-describedby="help1" />
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 align-items-center">
        <div class="col-2">
          <label for="spam" class="col-form-label"><?php print $text_spam; ?></label>
        </div>
        <div class="col-5">
          <select class="form-control" name="spam">
            <option value="-1">-</option>
            <option value="0"><?php print $text_not_spam; ?></option>
            <option value="1"><?php print $text_spam2; ?></option>
          </select>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      <div class="row g-3 mt-2 align-items-center">
        <div class="col-2">
        </div>
        <div class="col-2">
          <button type="submit" class="btn btn-primary"><?php print $text_add; ?></button>
          <button type="reset" class="btn btn-secondary"><?php print $text_cancel; ?></button>
        </div>
        <div class="col-auto">
          <button type="button" class="btn btn-danger" onclick="Piler.reload_piler();"><?php print $text_apply_changes; ?></button>
        </div>
        <div class="col-auto">
          <span id="help1" class="form-text"></span>
        </div>
      </div>

      </form>
    </div>
  </div>
</div>


<div class="container text-start mt-5">
  <div class="row">
    <div class="col">

      <h4><?php print $text_existing_rules; ?></h4>

      <table class="table table-striped">
        <thead class="table-dark">
          <tr>
            <th><?php print $text_from; ?> <a href="<?php print $MEURL; ?>&sort=from"><i class="bi bi-arrow-down-up"></i></a></th>
            <th><?php print $text_to; ?> <a href="<?php print $MEURL; ?>&sort=to"><i class="bi bi-arrow-down-up"></i></a></th>
            <th><?php print $text_subject; ?> <a href="<?php print $MEURL; ?>&sort=subject"><i class="bi bi-arrow-down-up"></i></a></th>
            <th><?php print $text_body; ?></th>
            <th><?php print $text_spam; ?></th>
            <th><?php print $text_size; ?></th>
            <th><?php print $text_attachment_name; ?></th>
            <th><?php print $text_attachment_type; ?></th>
            <th><?php print $text_attachment_size; ?></th>
            <th>&nbsp;</th>
          </tr>
        </thead>
        <tbody>
        <?php foreach($rules as $rule) { ?>
          <tr>
            <td><?php print htmlentities($rule['from']); ?></td>
            <td><?php print htmlentities($rule['to']); ?></td>
            <td><?php print htmlentities($rule['subject']); ?></td>
            <td><?php print htmlentities($rule['body']); ?></td>
            <td><?php if($rule['spam'] == -1) { print "-"; } else if($rule['spam'] == 0) { print $text_not_spam; } else { print $text_spam; } ?></td>
            <td><?php if($rule['size'] > 0) { print $rule['_size']; ?> <?php print $rule['size']; } ?></td>
            <td><?php print htmlentities($rule['attachment_name']); ?></td>
            <td><?php print htmlentities($rule['attachment_type']); ?></td>
            <td><?php if($rule['attachment_size'] > 0) { print $rule['_attachment_size']; ?> <?php print $rule['attachment_size']; } ?></td>
            <td><a href="<?php print PATH_PREFIX; ?>index.php?route=policy/removeexclusion&amp;id=<?php print $rule['id']; ?>&amp;confirmed=1"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></a></td>
          </tr>
        <?php } ?>
        <tbody>
      </table>

    </div>
  </div>
</div>
