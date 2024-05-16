<div class="container text-danger text-start">
<?php if(isset($errorstring)) { print $text_error . ': ' . $errorstring; } ?>
</div>

<div class="container text-success text-start">
<?php if(isset($x)) { print $x; } ?>
</div>

<div class="container text-start">
  <div class="row">
    <div class="col">

      <h4><?php print $text_progress; ?></h4>

      <div class="listarea">

        <table id="ss1" class="table table-striped table-condensed">
          <tr>
            <th><?php print $text_type; ?></th>
            <th><?php print $text_username; ?></th>
            <th><?php print $text_server_name; ?></td>
            <th><?php print $text_progress; ?></td>
            <th><?php print $text_status; ?></td>
            <th>&nbsp;</th>
          </tr>

        <?php foreach($entries as $e) { ?>
          <tr>
            <td><?php print $e['type']; ?></td>
            <td><?php print $e['username']; ?></td>
            <td><?php print $e['server']; ?></td>
            <td><?php print $e['imported']; ?> / <?php print $e['total']; ?></td>
            <td><?php print $import_status[$e['status']]; ?></td>
            <td><a href="index.php?route=import/remove&amp;id=<?php print $e['id']; ?>&amp;confirmed=1"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></i></a></td>
          </tr>
        <?php } ?>

        </table>

      </div>



    </div>
  </div>
</div>
