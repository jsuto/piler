<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_import_job_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=import/remove&amp;id=-1&amp;name=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<?php if($id == -1) { ?>

<h4><?php print $text_progress; ?></h4>

<div class="listarea">

<?php if(isset($entries)){ ?>

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
         <td><a href="index.php?route=import/remove&amp;id=<?php print $e['id']; ?>&amp;name=<?php print urlencode($e['username']); ?>&amp;confirmed=1" class="confirm-delete" data-id="<?php print $e['id']; ?>" data-name="<?php print $e['username']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
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

