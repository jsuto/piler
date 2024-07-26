  <!-- auditor restore modal -->

  <div id="restoreModal" class="modal" tabindex="-1">
    <div class="modal-dialog modal-lg">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title"><?php print $text_restore_message; ?></h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body mb-3">
          <label for="FormControlInput1" class="form-label"><?php print $text_email; ?></label>
          <input type="text" class="form-control" id="restore_address" name="restore_address" />
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_cancel; ?></button>
          <button type="button" class="btn btn-primary" onclick="let id=$('#xid').val(); Piler.restore_message_for_recipient(id, $('#restore_address').val(), '<?php print $text_restored; ?>', 'ERROR');"><?php print $text_restore; ?></button>
        </div>
      </div>
    </div>
  </div>


  <!-- auditor bulk restore modal -->

  <div id="bulkRestoreModal" class="modal" tabindex="-1">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title"><?php print $text_restore; ?></h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body mb-3">
          <label for="FormControlInput1" class="form-label"><?php print $text_forward_selected_emails_to; ?></label>
          <input type="text" class="form-control" id="bulk_restore_address" name="bulk_restore_address" />
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_cancel; ?></button>
          <button type="button" class="btn btn-primary" onclick="let addr = $('#bulk_restore_address').val(); if(addr) { Piler.bulk_restore_messages('<?php print $text_restored; ?>', addr); }"><?php print $text_restore; ?></button>
        </div>
      </div>
    </div>
  </div>



  <!-- advanced search modal -->

  <div class="modal fade" id="advancedSearchModal" tabindex="-1" aria-labelledby="advancedSearchModalLabel" aria-hidden="true">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h1 class="modal-title fs-5" id="advancedSearchModalLabel"><?php print $text_advanced_search; ?></h1>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body">

          <input type="hidden" name="xsearchtype" id="xsearchtype" value="simple" />
          <input type="hidden" name="xsort" id="xsort" value="date" />
          <input type="hidden" name="xorder" id="xorder" value="0" />
          <input type="hidden" name="xref" id="xref" value="" />

          <table class="table" aria-describedby="advanced search" role="presentation">
            <tr>
              <td><?php print $text_from; ?></td>
              <td><input type="text" class="form-control" name="xfrom" id="xfrom" value="<?php if(isset($from)) { print $from; } ?>" v-model="complex_search.from" /></td>
            </tr>

            <tr>
              <td><?php print $text_to; ?></td>
              <td><input type="text" class="form-control" name="xto" id="xto" value="<?php if(isset($to)) { print $to; } ?>" v-model="complex_search.to" /></td>
            </tr>

            <tr>
              <td><?php print $text_subject; ?></td>
              <td><input type="text" class="form-control" name="xsubject" id="xsubject" value="<?php if(isset($subject)) { print $subject; } ?>" v-model="complex_search.subject" /></td>
            </tr>

            <tr>
              <td><?php print $text_body; ?></td>
              <td><input type="text" class="form-control" name="xbody" id="xbody" value="<?php if(isset($body)) { print $body; } ?>" v-model="complex_search.body" /></td>
            </tr>

            <tr>
              <td><?php print $text_tags; ?></td>
              <td><input type="text" class="form-control" name="xtag" id="xtag" value="<?php if(isset($tag)) { print $tag; } ?>" v-model="complex_search.tag" /></td>
            </tr>

            <tr>
              <td><?php print $text_notes; ?></td>
              <td><input type="text" class="form-control" name="xnote" id="xnote" value="<?php if(isset($note)) { print $note; } ?>" v-model="complex_search.note" /></td>
            </tr>
            <tr>
              <td><?php print $text_attachment; ?></td>
              <td>
                <img src="<?php print ICON_DOC; ?>" alt="Word" title="Word" /> <input type="checkbox" class="form-check-input align-middle" name="xhas_attachment_doc" value="word" v-model="complex_search.attachments" id="xhas_attachment_doc" />&nbsp;
                <img src="<?php print ICON_XLS; ?>" alt="Excel" title="Excel" /> <input type="checkbox" class="form-check-input align-middle" name="xhas_attachment_xls" value="excel" v-model="complex_search.attachments" id="xhas_attachment_xls" />&nbsp;
                <img src="<?php print ICON_PDF; ?>" alt="PDF" title="PDF" /> <input type="checkbox" class="form-check-input align-middle" name="xhas_attachment_pdf" value="pdf" v-model="complex_search.attachments" id="xhas_attachment_pdf" />&nbsp;
                <img src="<?php print ICON_IMAGE; ?>" alt="image" title="image" /> <input type="checkbox" class="form-check-input align-middle" name="xhas_attachment_image" value="image" v-model="complex_search.attachments" id="xhas_attachment_image" />&nbsp;
                <img src="<?php print ICON_FILE; ?>" alt="any" title="any" /> <input type="checkbox" class="form-check-input align-middle" name="xhas_attachment_any" value="any" v-model="complex_search.attachments" id="xhas_attachment_any" />
              </td>
            </tr>
            <tr>
              <td><?php print $text_date_from; ?></td>
              <td><input id="date1" name="date1" class="form-control" type="date" v-model="complex_search.date1" /></td>
            </tr>

            <tr>
              <td><?php print $text_date_to; ?></td>
              <td><input id="date2" name="date2" class="form-control" type="date" v-model="complex_search.date2" /></td>
            </tr>
          </table>

        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
          <button type="button" class="btn btn-primary" onclick="Piler.complex();"><?php print $text_search; ?></button>
        </div>
      </div>
    </div>
  </div>


  <!-- delete modal -->

  <div id="deleteModal" class="modal" tabindex="-1">
    <div class="modal-dialog">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title"><?php print $text_delete; ?></h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body mb-3">
          <label for="FormControlInput1" class="form-label"><?php print $text_delete_reason; ?></label>
          <input type="text" id="reason" class="form-control" />
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_cancel; ?></button>
          <button type="button" class="btn btn-danger" onclick="let reason = $('#reason').val(); if(reason) { Piler.bulk_remove_messages(reason, '<?php if(NEED_TO_APPROVE_DELETE) { print $text_need_to_approve_removal; } else { print $text_successfully_removed; } ?>', '<?php print $text_no_selected_message; ?>'); } else { Piler.show_message('ERROR', 'Please give a reason to delete this message'); } "><?php print $text_delete; ?></button>
        </div>
      </div>
    </div>
  </div>

  <!-- preview modal -->

  <div id="previewMessageModal" class="modal" tabindex="-1">
    <div class="modal-dialog modal-fullscreen">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title"><?php print $text_restore; ?></h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body mb-3">
          <span id="preview_modal"></span>
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
        </div>
      </div>
    </div>
  </div>
