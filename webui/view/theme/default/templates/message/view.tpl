<div id="restorebox">
<?php if(Registry::get('auditor_user') == 1 && count($rcpt) > 0) { ?>
<?php foreach($rcpt as $r) { ?>
      <input type="checkbox" class="restorebox" id="rcpt_<?php print $r; ?>" name="rcpt_<?php print $r; ?>" value="1" /> <?php print $r; ?><br />
<?php } ?>
<br />
<input type="button" id="restore_button" name="restore_button" value="<?php print $text_restore; ?>" class="restore_to_mailbox_button" onclick="Piler.restore_message_for_recipients(<?php print $id; ?>, '<?php print $text_restored; ?>', '<?php print $text_select_recipients; ?>');" />
<input type="button" value="<?php print $text_cancel; ?>" class="restore_to_mailbox_button" onclick="$('#restorebox').hide();" />
<?php } ?>

</div>

<p>
   <a class="messagelink" href="index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
<?php if(Registry::get('auditor_user') == 1) { ?>
   <a class="messagelink" href="#" onclick="$('#restorebox').show();"><?php print $text_restore_to_mailbox; ?></a> |
<?php } else { ?>
   <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><?php print $text_restore_to_mailbox; ?></a> |
<?php } ?>
   <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><?php print $text_view_headers; ?></a>
</p>

<strong><?php if($message['subject'] == "" || $message['subject'] == "Subject:") { print "&lt;" . $text_no_subject . "&gt;"; } else { print $message['subject']; } ?></strong><br />
<strong><?php print $message['from']; ?></strong><br />
<strong><?php print $message['to']; ?></strong><br />
<strong><?php print $message['date']; ?></strong><br />

<form class="form-inline" name="view">

<strong><?php print $text_notes; ?></strong>: <input type="text" size="60" id="note" name="note" class="input-medium" value="<?php print preg_replace("/\"/", "&quot;", $message['note']); ?>" /> <input type="button" value="<?php print $text_save; ?>" class="btn btn-warning" onclick="Piler.add_note_to_message(<?php print $id; ?>, '<?php print $text_saved; ?>'); " />

</form>

<?php foreach($attachments as $a) { ?>
   <span><img src="<?php print ICON_ATTACHMENT; ?>" /><a href="index.php?route=message/attachment&id=<?php print $a['id']; ?>"><?php print $a['name']; ?></a></span>
<?php } ?>

<hr />
<?php print $message['message']; ?><br />
