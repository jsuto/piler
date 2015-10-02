
<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>

<form action="index.php?route=folder/edit" method="post" name="extra_folders" class="form-horizontal">

    <div class="control-group">
		<label class="control-label" for="domain"><?php print $text_folder; ?>:</label>
        <div class="controls">
            <input type="text" id="name" name="name" value="<?php print $folder['name']; ?>" />
            <input type="hidden" id="id" name="id" value="<?php print $folder['id']; ?>" />
        </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php print $text_modify; ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" />
    </div>

</form>


    <div id="search">

        <h4><?php print $text_existing_folders; ?></h4>

        <table id="search1" class="table table-striped table-condensed">
            <thead>
                <th><?php print $text_folder; ?></th>
                <th>&nbsp;</th>
            </thead>
            <tbody>
      <?php foreach($extra_folders as $folder) { ?>
                <tr>
                    <td><?php print $folder['name']; ?></td>
                    <td><a href="index.php?route=folder/edit&id=<?php print $folder['id']; ?>"><?php print $text_edit; ?></a></td>
                    <td><a href="/folders.php?id=<?php print $folder['id']; ?>&remove=1"><?php print $text_remove; ?></a></td>
                </tr>
      <?php } ?>
            </tbody>
        </table>

   </div>

<?php } ?>



