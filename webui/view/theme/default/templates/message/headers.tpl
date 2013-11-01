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
   <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><i class="icon-cloud-download"></i>&nbsp;<?php print $text_download_message; ?></a> |
<?php if(SMARTHOST) { if(Registry::get('auditor_user') == 1) { ?>
   <a class="messagelink" href="#" onclick="$('#restorebox').show();"><i class="icon-reply"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
<?php } else { ?>
   <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><i class="icon-reply"></i>&nbsp;<?php print $text_restore_to_mailbox; ?></a> |
<?php } } ?>
   <a class="messagelink" href="#" onclick="Piler.view_message(<?php print $id; ?>);"><i class="icon-envelope-alt"></i>&nbsp;<?php print $text_view_message; ?></a>
<?php if($message['has_journal'] == 1 && Registry::get('auditor_user') == 1 && SHOW_ENVELOPE_JOURNAL == 1) { ?>
   | <a class="messagelink" href="#" onclick="Piler.view_journal(<?php print $id; ?>);"><i class="icon-envelope-alt"></i>&nbsp;<?php print $text_view_journal_envelope; ?></a>
<?php } ?>

</p>

</div>

<pre class="messagesmtpheaders"><?php print $message['headers']; ?></pre>

