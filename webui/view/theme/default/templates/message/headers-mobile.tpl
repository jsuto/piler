<div id="restorebox" class="alert alert-general">
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
   <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="glyphicon glyphicon-cloud-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
<?php } ?>
<?php if(SMARTHOST || ENABLE_IMAP_AUTH == 1) { if(Registry::get('auditor_user') == 1) { ?>
   <a class="messagelink" href="#" onclick="$('#restorebox').show();"><i class="glyphicon glyphicon-refresh"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
<?php } else if($can_restore == 1) { ?>
   <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><i class="glyphicon glyphicon-refresh"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
<?php } } ?>
   <a class="messagelink" href="#" onclick="Piler.view_message(<?php print $id; ?>);"><i class="glyphicon glyphicon-envelope"></i>&nbsp;<?php print $text_view_message; ?></a>

<?php if(ENABLE_PDF_DOWNLOAD) { ?>
   | <a class="messagelink" href="index.php?route=message/pdf&amp;id=<?php print $id; ?>"><img src="/view/theme/default/assets/images/fileicons/pdf.png" />&nbsp;PDF</a>
<?php } ?>

<?php if($message['has_journal'] == 1 && Registry::get('auditor_user') == 1 && SHOW_ENVELOPE_JOURNAL == 1) { ?>
   | <a class="messagelink" href="#" onclick="Piler.view_journal(<?php print $id; ?>);"><i class="glyphicon glyphicon-envelope"></i>&nbsp;<?php print $text_view_journal_envelope; ?></a>
<?php } ?>
   | <a href="#" onclick="($('#notesbox').is(':hidden'))?$('#notesbox').show():$('#notesbox').hide();"><i class="glyphicon glyphicon-file"></i>&nbsp;<?php print $text_create_note; ?></a>
</p>

</div>

<div id="notesbox" class="input-prepend input-append" style="display:none;">
   <span class="add-on"><i class="icon-file-alt icon-large"></i>&nbsp;<?php print $text_notes; ?>:</span>
   <input type="text" size="60" id="note" name="note" class="mynote" value="<?php print preg_replace("/\"/", "&quot;", $message['note']); ?>" />
   <input type="button" class="btn btn-info" value="<?php print $text_save; ?>" class="message_button" onclick="Piler.add_note_to_message(<?php print $id; ?>, '<?php print $text_saved; ?>');" />
</div>

<pre class="messagesmtpheaders"><?php print $message['headers']; ?></pre>

