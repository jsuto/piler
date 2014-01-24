<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p><?php print $text_delete_confirm_message; ?> <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=search/autosearch&amp;remove=1&amp;id=-1&amp;&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

<!--form method="get" name="search1" action="index.php?route=search/autosearch" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form-->

<h4><?php print $text_new; ?></h4>

<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>
<?php if(isset($x)){ ?>
    <div class="alert alert-success"><?php print $x; ?></div>
<?php } ?>

<form method="post" name="add1" action="index.php?route=search/autosearch" class="form-horizontal">

    <div class="control-group<?php if(isset($errors['mapped'])){ print " error"; } ?>">
       <label class="control-label" for="mapped"><?php print $text_search; ?>:</label>
       <div class="controls">
          <input type="text" class="text span6" name="query" placeholder="Search query"<?php if(isset($post['query'])){ echo " value='".$post['query']."'"; } ?> />
          <?php if ( isset($errors['query']) ) { ?><span class="help-inline"><?php print $errors['query']; ?></span><?php } ?>
       </div>
    </div>


    <div class="form-actions">
        <input type="submit" value="<?php print $text_add; ?>" class="btn btn-primary" />
        <input type="reset" value="<?php print $text_clear; ?>" class="btn" />
    </div>

</form>

<h4><?php print $text_existing; ?></h4>

<div class="listarea">

<?php if(isset($data) && count($data) > 0){ ?>

   <table id="ss1" class="table table-striped table-condensed">
      <tr>
         <th><?php print $text_search; ?></th>
         <th>&nbsp;</th>
      </tr>

<?php foreach($data as $d) { ?>
      <tr>
         <td><?php print $d['query']; ?></td>
         <td><a href="index.php?route=search/autosearch&amp;remove=1&amp;id=<?php print $d['id']; ?>&amp;query=<?php print urlencode($d['query']); ?>" class="confirm-delete" data-id="<?php print $d['id']; ?>" data-name="<?php print $d['query']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
      </tr>
<?php } ?>

   </div>

<?php } else { ?>
    <div class="alert alert-error lead">
    <?php print $text_not_found; ?>
    </div>
<?php } ?>

</div>

<?php if($remove == 1) { ?>
   <script>Piler.show_message('messagebox1', '<?php print $text_successfully_removed; ?>', 0.85);</script>
<?php } ?>

