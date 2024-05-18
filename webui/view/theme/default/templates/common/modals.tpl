<!-- modals -->

<div id="restoreModal" class="modal" tabindex="-1">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title"><?php print $text_restore; ?></h5>
        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
      </div>
      <div class="modal-body">
        <input type="text" class="form-control" id="restore" name="restore">
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
        <a href="#" onclick="let xid=$('#xid').val(); let addr = $('#restore').val(); if(addr) { Piler.restore_message_for_recipient(xid, addr, '<?php print $text_restored; ?>', '<?php print $text_failed_to_restore; ?>'); }" class="btn btn-primary">OK</a>
      </div>
    </div>
  </div>
</div>


<div id="bulkRestoreModal" class="modal" tabindex="-1">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title"><?php print $text_forward_selected_emails_to; ?></h5>
        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
      </div>
      <div class="modal-body">
        <input type="text" class="form-control" id="restore_address" name="restore_address">
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
        <a href="#" onclick="var addr = $('#restore_address').val(); if(addr) { Piler.bulk_restore_messages('<?php print $text_restored; ?>', addr); hide_modal('bulkRestoreModal'); }" class="btn btn-primary">OK</a>
      </div>
    </div>
  </div>
</div>


<div id="deleteModal" class="modal" tabindex="-1">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title"><?php print $text_delete_reason; ?></h5>
        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
      </div>
      <div class="modal-body">
        <input type="text" class="form-control" id="reason" name="reason">
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
        <a href="#" onclick="var reason = $('#reason').val(); if(reason) { Piler.bulk_remove_messages(reason, '<?php if(NEED_TO_APPROVE_DELETE) { print $text_need_to_approve_removal; } else { print $text_successfully_removed; } ?>', '<?php print $text_no_selected_message; ?>'); hide_modal('deleteModal');}" class="btn btn-primary">OK</a>
      </div>
    </div>
  </div>
</div>
