<input type="hidden" id="xid" value="<?php print $id; ?>" />

<div class="messageheader">

<p>
<?php if($can_download == 1) { ?>
   <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="bi bi-cloud-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
<?php } ?>
<?php if(SMARTHOST || ENABLE_IMAP_AUTH == 1) { if(Registry::get('auditor_user') == 1) { ?>
   <a class="messagelink" href="#" data-bs-toggle="modal" data-bs-target="#restoreModal"><i class="bi bi-arrow-90deg-right"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
<?php } else if($can_restore == 1) { ?>
   <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><i class="bi bi-arrow-90deg-right"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
<?php } } ?>
   <a class="messagelink" href="#" onclick="Piler.view_message(<?php print $id; ?>);"><i class="bi bi-envelope"></i>&nbsp;<?php print $text_view_message; ?></a>

<?php if(ENABLE_PDF_DOWNLOAD) { ?>
   | <a class="messagelink" href="index.php?route=message/pdf&amp;id=<?php print $id; ?>"><img src="/view/theme/default/assets/images/fileicons/pdf.png" />&nbsp;<?php print "PDF"; ?></a>
<?php } ?>

<?php if($message['has_journal'] == 1 && Registry::get('auditor_user') == 1 && SHOW_ENVELOPE_JOURNAL == 1) { ?>
   | <a class="messagelink" href="#" onclick="Piler.view_journal(<?php print $id; ?>);"><i class="bi bi-envelope"></i>&nbsp;<?php print $text_view_journal_envelope; ?></a>
<?php } ?>

</p>

</div>

<pre class="messagesmtpheaders"><?php print $message['headers']; ?></pre>
