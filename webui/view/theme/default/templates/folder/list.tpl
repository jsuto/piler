
<?php if(!isset($x)){ ?>

<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>

<form action="folders.php" method="post" name="extra_folders">

   <div id="search">

      <div id="search1">

         <div class="row">
            <div class="cell1"><?php print $text_folder; ?></div>
            <div class="cell2"><input type="text" id="name" name="name" value="" /></div>
         </div>

         <div class="row">
            <div class="cell1">&nbsp;</div>
            <div class="cell2"><input type="submit" value="<?php print $text_add; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></div>
         </div>

      </div>

   </div>

</form>


   <div id="search">

<h4><?php print $text_existing_folders; ?></h4>

      <div id="search1">

      <?php foreach($extra_folders as $folder) { ?>
         <div class="row">
            <div class="cell1"><?php print $folder['name']; ?></div>
            <div class="cell2"><a href="/folders.php?id=<?php print $folder['id']; ?>&remove=1"><?php print $text_remove; ?></a></div>
         </div>
      <?php } ?>


      </div>

   </div>

<?php } ?>


<?php } else { ?>
<?php print $x; ?>. <a href="index.php?route=common/home"><?php print $text_back; ?></a>
<?php } ?>


