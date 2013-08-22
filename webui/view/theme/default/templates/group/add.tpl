
<?php if(isset($errorstring)){ ?><div class="alert alert-danger"><?php print $text_error; ?>: <?php print $errorstring; ?></div><?php } ?>

<?php if(isset($x)){ ?>

<div class="alert alert-success"><?php print $x; ?>.</div>
<p><a href="index.php?route=group/list"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<?php } else { ?>

<p><a href="index.php?route=group/list"><i class="icon-circle-arrow-left"></i>&nbsp;<?php print $text_back; ?></a></p>

<form action="index.php?route=group/add" name="addgroup" method="post" autocomplete="off" class="form-horizontal">

    <div class="control-group">
        <label class="control-label" for="groupname"><?php print $text_groupname; ?>:</label>
        <div class="controls">
            <input type="text" name="groupname" id="groupname" value="<?php if(isset($post['groupname'])){ print $post['groupname']; } ?>" class="text" />
        </div>
    </div>
   
    <div class="control-group">
        <label class="control-label" for="s_piler_email"><?php print $text_search_emails; ?>*:</label>
        <div class="controls">
            <input type="text" id="s_piler_email" name="s_piler_email" placeholder="<?php print $text_search_email_to_add; ?>" class="autocompletetext" />
        </div>
    </div>

    <div class="control-group">
	  <label class="control-label" for="throwaway"><?php print $text_select_letter; ?>:</label>
	  <div class="controls">
	<?php foreach(Registry::get('letters') as $letter) { ?>
         <a href="#" onclick="window.open('<?php print SITE_URL; ?>index.php?route=group/email&term=<?php print $letter; ?>', 'aaa', 'width=300,height=400,scroll=yes');" ><?php print $letter; ?></a>
<?php } ?>
	  </div>
    </div>
    
    <div class="control-group">
        <label class="control-label" for="s_piler_email"><?php print $text_email_addresses; ?>**:</label>
        <div class="controls">
            <textarea style="height:280px;" name="email" id="email" class="domain"><?php if(isset($post['email'])){ print $post['email']; } ?></textarea>
        </div>
    </div>

    <div class="alert alert-info">*: <?php print $text_min_2_chars; ?><br />**: <?php print $text_enter_one_email_address_per_line; ?></div>

    <div class="form-actions">
        <input type="submit" id="button_add" class="btn btn-primary" value="<?php print $text_add; ?>" />
        <a href="index.php?route=group/list" class="btn"><?php print $text_cancel; ?></a>
    </div>
    
</form>

<?php } ?>

