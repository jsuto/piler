
<?php if(!isset($x)){ ?>

<?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>

<form action="folders.php" method="post" name="extra_folders" class="form-horizontal">

    <div class="control-group">
		<label class="control-label" for="domain"><?php print $text_folder; ?>:</label>
        <div class="controls">
            <input type="text" id="name" name="name" value="" />
        </div>
    </div>

    <div class="form-actions">
        <input type="submit" value="<?php print $text_add; ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" />
    </div>

</form>


    <div id="search">

        <h4><?php print $text_existing_folders; ?></h4>

        <table id="search1" class="table table-striped table-condensed">
            <thead>
                <th><?php print $_['text_folder']; ?> <?php print $_['text_name']; ?></th>
                <th>&nbsp;</th>
            </thead>
            <tbody>
      <?php foreach($extra_folders as $folder) { ?>
                <tr>
                    <td><?php print $folder['name']; ?></td>
                    <td><a href="/folders.php?id=<?php print $folder['id']; ?>&remove=1"><?php print $text_remove; ?></a></td>
                </tr>
      <?php } ?>
            </tbody>
        </table>

   </div>

<?php } ?>

<?php } else { ?>
<div class="alert alert-info"><?php print $x; ?>.</div>
<p><a href="index.php?route=common/home"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>
<?php } ?>


