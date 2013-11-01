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
<?php if(SMARTHOST) { if(Registry::get('auditor_user') == 1) { ?>
   <a class="messagelink" href="#" onclick="$('#restorebox').show();"><?php print $text_restore_to_mailbox; ?></a> |
<?php } else { ?>
   <a class="messagelink" href="#" onclick="Piler.restore_message(<?php print $id; ?>);"><?php print $text_restore_to_mailbox; ?></a> |
<?php } } ?>
   <a class="messagelink" href="#" onclick="Piler.view_headers(<?php print $id; ?>);"><?php print $text_view_headers; ?></a> |
   <a class="messagelink" href="#" onclick="Piler.view_message(<?php print $id; ?>);"><?php print $text_view_message; ?></a>
</p>

<pre><?php print $data; ?></pre>

