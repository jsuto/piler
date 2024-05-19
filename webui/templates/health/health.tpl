<?php if(EXTERNAL_DASHBOARD_URL) { ?>
   <iframe width="100%" height="100%" frameborder="0" src="<?php print EXTERNAL_DASHBOARD_URL; ?>"></iframe>
<?php } else { ?>
   <span id="A1"><div id="spinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_loading; ?></div></span>
<?php } ?>
