<p style="text-align:left; font-weight: bold; "><?php print $text_folders; ?>:</p>

<div id="folders" style="text-align:left;">

<?php foreach ($folders as $folder) { ?>
   <input type="checkbox" id="folder_<?php print $folder['id']; ?>" name="folder_<?php print $folder['id']; ?>" /> <?php print $folder['name']; ?><br />

<?php } ?>

</div>


