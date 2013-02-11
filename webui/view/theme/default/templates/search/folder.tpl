<p style="text-align:left; font-weight: bold; "><?php print $text_folders; ?>:</p>

<div id="folders" style="text-align:left;">


<?php foreach ($extra_folders as $folder) { ?>
   <blockquote style="border: 0px solid red; margin: 0 0 5px 10px;">
      <img src="<?php print ICON_EMPTY; ?>" width="12" height="12" alt="" /> <input type="checkbox" id="extra_folder_<?php print $folder['id']; ?>" name="extra_folder_<?php print $folder['id']; ?>" /> <input type="text" ondrop="copy_message_to_folder('<?php print $folder['id']; ?>', current_message_id, '<?php print $text_copied; ?>'); return false;" class="extra_folder" value="<?php print $folder['name']; ?>" />
   </blockquote>
<?php } ?>



<?php

function display_folders($arr = array(), &$i) {

?>
   <blockquote id="fldr_<?php print $i; ?>" style="border: 0px solid red; margin: 0 0 5px 10px;">
<?php
   $i++;

   foreach($arr as $a) {
?>

      <?php if(count($a['children']) > 0) { ?>
         <a id="fldr_collapse_<?php print $i; ?>" href="#" onclick="close_folder(<?php print $i; ?>); return false;"><img src="<?php print ICON_MINUS; ?>" alt="" /></a>
         <a id="fldr_open_<?php print $i; ?>" href="#" onclick="open_folder(<?php print $i; ?>); return false;" style="display:none;"><img src="<?php print ICON_PLUS; ?>" alt="" /></a>
      <?php } else { ?> <img src="<?php print ICON_EMPTY; ?>" width="12" height="12" alt="" /> <?php } ?>
      <input type="checkbox" id="folder_<?php print $a['id']; ?>" name="folder_<?php print $a['id']; ?>" /> <?php print $a['name']; ?><br />

<?php
      if(count($a['children'])) { display_folders($a['children'], $i); }
   } 
?>

      </blockquote>
<?php
}
?>

<?php
      $i = 0;
      display_folders($folders_by_hier, $i);
?>


</div>
