  <!-- delete modal -->

  <div id="deleteModal" class="modal" tabindex="-1">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title"><?php print $text_delete_selected; ?></h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body mb-3">
          <label for="FormControlInput1" class="form-label"><?php print $text_delete_reason; ?></label>
          <input type="text" id="reason" class="form-control" />
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_cancel; ?></button>
          <button type="button" class="btn btn-danger" onclick="let reason = $('#reason').val(); if(reason) { Piler.bulk_remove_messages(reason, '<?php if(NEED_TO_APPROVE_DELETE) { print $text_need_to_approve_removal; } else { print $text_successfully_removed; } ?>', '<?php print $text_no_selected_message; ?>'); } hide_modal('deleteModal');"><?php print $text_delete; ?></button>
        </div>
      </div>
    </div>
  </div>
