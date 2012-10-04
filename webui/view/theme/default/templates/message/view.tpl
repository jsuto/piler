
<p>
   <a class="messagelink" href="/index.php?route=message/download&amp;id=<?php print $id; ?>"><?php print $text_download_message; ?></a> |
   <a class="messagelink" href="#" onclick="script:load_url_with_get('<?php print SITE_URL; ?>/index.php?route=message/restore&amp;id=<?php print $id; ?>', 'mailpreviewframe'); return false;"><?php print $text_restore_to_mailbox; ?></a> |
   <a class="messagelink" href="#" onclick="script:load_url_with_get('<?php print SITE_URL; ?>/index.php?route=message/headers&amp;id=<?php print $id; ?>', 'mailpreviewframe'); return false;"><?php print $text_view_headers; ?></a>
</p>

<strong><?php if($message['subject'] == "" || $message['subject'] == "Subject:") { print "&lt;" . $text_no_subject . "&gt;"; } else { print $message['subject']; } ?></strong><br />
<strong><?php print $message['from']; ?></strong><br />
<strong><?php print $message['to']; ?></strong><br />
<strong><?php print $message['date']; ?></strong> <?php print $text_notes; ?>: <input type="text" size="60" id="note" name="note" class="advtextgrey" style="width: 500px;margin: 0px; color: #000000; height:10px;" value="<?php print $message['note']; ?>" /> <input type="button" value="<?php print $text_save; ?>" style="vertical-align: middle; font: bold 11px Arial, sans-serif; height:16px;" onclick="javascript:var p = 'id=<?php print $id; ?>&note=' + encodeURI(document.getElementById('note').value); send_ajax_post_request('<?php print MESSAGE_NOTE_URL; ?>', p); show_message('messagebox1', '<p>SAVED</p>', 0.85); " /><br />

<?php foreach($attachments as $a) { ?>
   <span><img src="<?php print ICON_ATTACHMENT; ?>" /><a href="index.php?route=message/attachment&id=<?php print $a['id']; ?>"><?php print $a['name']; ?></a></span>
<?php } ?>

<hr />
<?php print $message['message']; ?><br />
