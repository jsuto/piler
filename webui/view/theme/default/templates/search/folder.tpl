<p style="text-align:left; font-weight: bold; "><?php print $text_folders; ?>:</p>

<div id="folders" style="text-align:left;">

<?php foreach ($folders as $folder) { ?>
   <input type="checkbox" id="folder_<?php print $folder['id']; ?>" name="folder_<?php print $folder['id']; ?>" /> <?php print $folder['name']; ?><br />
<?php } ?>


<?php foreach ($extra_folders as $folder) { ?>
   <input type="checkbox" id="extra_folder_<?php print $folder['id']; ?>" name="extra_folder_<?php print $folder['id']; ?>" /> <span style="color: blue; font-weight: bold;" onmouseover="javascript: copy_message_to_folder('<?php print $folder['id']; ?>', current_message_id, '<?php print $text_copied; ?>'); return false;"><?php print $folder['name']; ?></span><br />
<?php } ?>


</div>


