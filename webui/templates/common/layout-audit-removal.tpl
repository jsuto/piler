<!DOCTYPE html>
<html lang="<?php print DEFAULT_LANG; ?>">
<head>
  <title><?php print $title; ?></title>
  <meta http-equiv="content-type" content="text/html; charset=utf-8" />
  <meta http-equiv="Content-Language" content="<?php print DEFAULT_LANG; ?>" />
  <meta name="viewport" content="width=device-width, initial-scale=1.0">

  <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
  <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
  <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
  <meta name="rating" content="general" />
  <meta name="robots" content="all" />
  <meta http-equiv="x-ua-compatible" content="IE=edge">

  <?php print CSS_CODE; ?>
  <?php print CUSTOM_CSS; ?>

  <?php print BRANDING_FAVICON; ?>
</head>

<body>

<?php include_once DIR_BASE . 'templates/common/common.tpl'; ?>

<div id="removeApproveModal" class="modal" tabindex="-1">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title"><?php print $text_confirm; ?> <?php print $text_delete; ?></script></h5>
        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
        <button type="button" class="btn btn-danger" onclick="Piler.approve_message_removal();"><?php print $text_remove; ?></button>
      </div>
    </div>
  </div>
</div>

<div id="removeRejectModal" class="modal" tabindex="-1">
  <div class="modal-dialog">
    <div class="modal-content">
      <div class="modal-header">
        <h5 class="modal-title"><?php print $text_forward_selected_emails_to; ?></h5>
        <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
      </div>
      <div class="modal-body">
        <textarea class="form-control" id="reason2"></textarea>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_close; ?></button>
        <button type="button" class="btn btn-danger" onclick="Piler.reject_message_removal();"><?php print $text_remove; ?></button>
      </div>
    </div>
  </div>
</div>

  <div id="header">
    <?php print $menu; ?>
  </div>

  <!-- Main content area with resizable panes -->

  <div class="container-fluid container-panes mt-2">
    <div class="pane pane-upper">
      <?php print $content; ?>
    </div>
    <div class="resizer"></div>

    <div id="preview" class="pane pane-lower">
      <h2>Preview Pane</h2>
      <p>Click on a message to show</p>
    </div>
  </div>

<?php print JS_CODE; ?>
<script src="/assets/js/resizer.js"></script>

</body>
</html>
