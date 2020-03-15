<div id="restorebox" class="alert alert-general" style="display:none;">
<?php if(Registry::get('auditor_user') == 1 && count($rcpt) > 0) { ?>
<?php foreach($rcpt as $r) { ?>
      <input type="checkbox" class="restorebox" id="rcpt_<?php print $r; ?>" name="rcpt_<?php print $r; ?>" value="1" /> <?php print $r; ?><br />
<?php } ?>
<br />
<input type="button" id="restore_button" name="restore_button" value="<?php print $text_restore; ?>" class="btn btn-primary" onclick="Piler.restore_message_for_recipients(<?php print $id; ?>, '<?php print $text_restored; ?>', '<?php print $text_select_recipients; ?>');" />
<input type="button" value="<?php print $text_cancel; ?>" class="btn btn-inverse" onclick="$('#restorebox').hide();" />
<?php } ?>

</div>

<div class="messageheader">

    <p>
    <?php if($can_download == 1) { ?>
       <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="icon-cloud-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
    <?php } ?>
    <?php if(SMARTHOST || ENABLE_IMAP_AUTH == 1) { if(Registry::get('auditor_user') == 1) { ?>
       <a class="messagelink" href="#" onclick="$('#restorebox').show();"><i class="icon-reply"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
    <?php } else if($can_restore == 1) { ?>
       <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><i class="icon-reply"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
    <?php } } ?>
       <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><i class="icon-envelope-alt"></i>&nbsp;<?php print $text_view_headers; ?></a>

    <?php if(ENABLE_PDF_DOWNLOAD) { ?>
       | <a class="messagelink" href="index.php?route=message/pdf&amp;id=<?php print $id; ?>"><img src="/view/theme/default/assets/images/fileicons/pdf.png" />&nbsp;<?php print "PDF"; ?></a>
    <?php } ?>

    <?php if($message['has_journal'] == 1 && Registry::get('auditor_user') == 1 && SHOW_ENVELOPE_JOURNAL == 1) { ?>
       | <a class="messagelink" href="#" onclick="Piler.view_journal(<?php print $id; ?>);"><i class="icon-envelope-alt"></i>&nbsp;<?php print $text_view_journal_envelope; ?></a>
    <?php } ?>
    <?php if(Registry::get('auditor_user') == 1) { ?>
       | <a class="messagelink" href="#" onclick="Piler.mark_as_private(<?php print $id; ?>, <?php $message['private'] == 1 ? print "0" : print "1" ?>);"><i class="icon-bookmark<?php if($message['private'] == 0) { ?>-empty<?php } ?>"></i>&nbsp;<?php print "private"; ?></a>
    <?php } ?>
    <?php if($spam == 1) { ?>
       | <a class="messagelink" href="#" onclick="Piler.not_spam(<?php print $id; ?>);"><i class="icon-exclamation-sign"></i>&nbsp;<?php print $text_not_spam; ?></a>
    <?php } ?>
       | <a href="#" onclick="Piler.print_div('messageblock');"><i class="glyphicon glyphicon-print"></i>&nbsp;<?php print $text_print_message; ?></a>

    <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 0) {
       if ($message['verification'] == 1) { ?><?php print $text_verified_flag; ?> <i class="glyphicon glyphicon-ok-sign icon-large" title="<?php print $text_verified_flag; ?>"></i><?php } else { ?><?php print $text_unverified_flag; ?> <i class="glyphicon glyphicon-remove-sign icon-large" title="<?php print $text_unverified_flag; ?>"></i><?php }
    } ?>

       | <a href="#" onclick="($('#notesbox').is(':hidden'))?$('#notesbox').show():$('#notesbox').hide();"><i class="glyphicon glyphicon-file"></i>&nbsp;<?php print $text_create_note; ?></a>
    </p>
</div>

<div id="notesbox" class="input-prepend input-append" style="display:none;">
   <span class="add-on"><i class="icon-file-alt icon-large"></i>&nbsp;<?php print $text_notes; ?>:</span>
   <input type="text" size="60" id="note" name="note" class="mynote" value="<?php print preg_replace("/\"/", "&quot;", $message['note']); ?>" />
   <input type="button" class="btn btn-info" value="<?php print $text_save; ?>" class="message_button" onclick="Piler.add_note_to_message(<?php print $id; ?>, '<?php print $text_saved; ?>'); " />
</div>

<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
<div id="folderbox" class="input-prepend input-append">
   <span class="add-on"><i class="icon-folder-open-alt icon-large"></i>&nbsp;<?php print $text_folder; ?>:</span>
   <select name="folder_id" id="folder_id" class="span2">
         <option value="0" <?php if($folder_id == 0) { ?>selected="selected"<?php } ?>>---</option>
      <?php foreach ($folders as $folder) { ?>
         <option value="<?php print $folder['id']; ?>" <?php if($folder_id == $folder['id']) { ?>selected="selected"<?php } ?>><?php print $folder['name']; ?></option>
      <?php } ?>
   </select>
   <input type="button" class="btn btn-info" value="<?php print $text_save; ?>" class="message_button" onclick="Piler.update_message_folder(<?php print $id; ?>, '<?php print $text_saved; ?>'); " />
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
       <span><i class="attachment icon-paper-clip icon-large" title="Message Attachment"></i>&nbsp;<a href="index.php?route=message/attachment&id=<?php print $a['id']; ?>"><?php print $a['name']; ?></a></span>
    <?php } ?><?php if(count($attachments) > 1) { ?>| <a href="index.php?route=message/attachments&id=<?php print $id; ?>"><i class="icon-briefcase"></i></a><?php } ?><br/>
</div>
<div class="messagecontents">
<?php print $message['message']; ?>
</div>

<?php foreach($images as $img) { ?>
   <p><img src="<?php print SITE_URL; ?>/tmp/<?php print $img['name']; ?>" alt="" /></p>
<?php } ?>


</div>

