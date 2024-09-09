<input type="hidden" id="xid" value="<?php print $id; ?>" />

<div class="container-fluid text-start">
  <div class="row">
    <div class="<?php if(FULL_GUI) { ?>col-8<?php } else { ?>col-12<?php } ?> text-start">

      <div id="notesbox" class="row g-3 align-items-center">
        <div class="col">

       <label class="col-form-label">
<?php if(FULL_GUI) { ?>

    <?php if($can_download == 1) { ?>
       <a class="messagelink" href="<?php print PATH_PREFIX; ?>index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="bi bi-cloud-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
    <?php } ?>
    <?php if(SMARTHOST || ENABLE_IMAP_AUTH == 1) { if(Registry::get('auditor_user') == 1) { ?>
       <a class="messagelink" href="#" data-bs-toggle="modal" data-bs-target="#restoreModal"><i class="bi bi-arrow-90deg-right"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
    <?php } else if($can_restore == 1) { ?>
       <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><i class="bi bi-arrow-90deg-right"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
    <?php } } ?>
       <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><i class="bi bi-envelope"></i>&nbsp;<?php print $text_view_headers; ?></a>

    <?php if(ENABLE_PDF_DOWNLOAD) { ?>
       | <a class="messagelink" href="<?php print PATH_PREFIX; ?>index.php?route=message/pdf&amp;id=<?php print $id; ?>"><i class="bi bi-file-pdf"></i>&nbsp;<?php print "PDF"; ?></a>
    <?php } ?>

    <?php if($message['has_journal'] == 1 && Registry::get('auditor_user') == 1 && SHOW_ENVELOPE_JOURNAL == 1) { ?>
       | <a class="messagelink" href="#" onclick="Piler.view_journal(<?php print $id; ?>);"><i class="bi bi-envelope-alt"></i>&nbsp;<?php print $text_view_journal_envelope; ?></a>
    <?php } ?>
    <?php if(Registry::get('auditor_user') == 1) { ?>
       | <a class="messagelink" href="#" onclick="Piler.mark_as_private(<?php print $id; ?>, <?php $message['private'] == 1 ? print "0" : print "1" ?>);"><?php if($message['private'] == 1) { ?><i class="bi bi-incognito"></i>&nbsp;<?php } ?>private</a>
    <?php } ?>
    <?php if($spam == 1) { ?>
       | <a class="messagelink" href="#" onclick="Piler.not_spam(<?php print $id; ?>);"><i class="bi bi-exclamation-sign"></i>&nbsp;<?php print $text_not_spam; ?></a>
    <?php } ?>
       | <a href="#" onclick="Piler.print_div('messageblock');"><i class="bi bi-printer"></i>&nbsp;<?php print $text_print_message; ?></a>

    <?php if ($message['verification'] == 1) { print $text_verified_flag; ?> <i class="verified bi bi-check-circle-fill text-success" title="<?php print $text_verified_flag; ?>"></i><?php } else { ?><?php print $text_unverified_flag; ?> <i class="unverified bi bi-x-square-fill text-danger" title="<?php print $text_unverified_flag; ?>"></i><?php } ?>

<?php } else { // full gui ?>
   <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><i class="bi bi-envelope"></i>&nbsp;<?php print $text_view_headers; ?></a>

   <?php if($can_restore == 1) { ?>
     <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><i class="bi bi-arrow-90deg-right"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a>
   <?php } ?>

   <?php if ($message['verification'] == 1) { ?><i class="verified bi bi-check-circle-fill text-success"></i><?php } else { ?><i class="unverified bi bi-x-square-fill text-danger"></i><?php } ?>

<?php } ?>


        </label>

        </div>
      </div>

    </div>

    <div class="col text-end">
    <?php if(FULL_GUI) { ?>
      <div id="notesbox" class="row g-3 align-items-center">
        <div class="col">
          <label for="note" class="col-form-label"><i class="bi bi-sticky fs-5"></i> <?php print $text_notes; ?>:</label>
        </div>
        <div class="col-auto">
          <input type="text" id="note" name="note" value="<?php H($message['note']); ?>" class="form-control" aria-describedby="notes">
        </div>
        <div class="col-auto">
          <span id="notes" class="form-text">
            <button class="btn btn-primary btn-sm" onclick="Piler.add_note_to_message(<?php print $id; ?>, '<?php print $text_saved; ?>'); "><?php print $text_save; ?></button>
          </span>
        </div>
      </div>
    <?php } ?>

    </div>
  </div>
</div>

<?php if(FULL_GUI && isset($tsa)) { ?>
<div class="container-fluid text-start tsa">
  <div class="row">
    <div class="col text-start">
    <?php foreach($tsa as $t) {
       print $t . '<br />';
    } ?>
    </div>
  </div>
</div>
<?php } ?>

<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
<div id="folderbox" class="input-prepend input-append">
   <span class="add-on"><i class="icon-folder-open-alt icon-large"></i>&nbsp;<?php print $text_folder; ?>:</span>
   <select name="folder_id" id="folder_id" class="span2">
         <option value="0" <?php if($folder_id == 0) { ?>selected="selected"<?php } ?>>---</option>
      <?php foreach ($folders as $folder) { ?>
         <option value="<?php print $folder['id']; ?>" <?php if($folder_id == $folder['id']) { ?>selected="selected"<?php } ?>><?php print $folder['name']; ?></option>
      <?php } ?>
   </select>
   <button class="btn btn-info" onclick="Piler.update_message_folder(<?php print $id; ?>, '<?php print $text_saved; ?>'); "><?php print $text_save; ?></button>
</div>
<?php } ?>

<div id="messageblock">

  <div class="messageheader">
    <strong><?php if($message['subject'] == "" || $message['subject'] == "Subject:") { print "&lt;" . $text_no_subject . "&gt;"; } else { print $message['subject']; } ?></strong><br />
    <strong><?php print $message['from']; ?></strong><br />
    <strong><?php print $message['to']; ?></strong><br />
    <?php if(strlen($message['cc']) > 6) { ?><strong><?php print $message['cc']; ?></strong><br /><?php } ?>
    <strong><?php print $message['date']; ?></strong><br />
    <?php foreach($attachments as $a) { ?>
       <span><i class="attachment icon-paper-clip icon-large" title="Message Attachment"></i>&nbsp;<a href="<?php print PATH_PREFIX; ?>index.php?route=message/attachment&id=<?php print $a['id']; ?>"><?php print $a['name']; ?></a></span>
    <?php } ?><?php if(count($attachments) > 1) { ?>| <a href="<?php print PATH_PREFIX; ?>index.php?route=message/attachments&id=<?php print $id; ?>"><i class="icon-briefcase"></i></a><?php } ?><br/>
  </div>
  <div class="messagecontents">
    <?php print $message['message']; ?>
  </div>

<?php foreach($images as $img) { ?>
   <p><img src="/tmp/<?php print $img['name']; ?>" alt="" /></p>
<?php } ?>


</div>
