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
<?php include_once DIR_BASE . 'templates/search/modals.tpl'; ?>

  <div id="header">
    <?php print $menu; ?>
  </div>

    <div class="searchcontainer">
      <div class="row w-100 ms-0">

        <div class="container text-center">
          <div class="row align-items-center">
            <div class="col<?php if(!FULL_GUI) { ?>-10<?php } ?> ps-0">
              <div class="d-flex align-items-center justify-content-start">
                <input type="hidden" name="searchtype" id="searchtype" value="expert" />
                <input type="hidden" name="sort" id="sort" value="date" />
                <input type="hidden" name="order" id="order" value="0" />
                <input type="text" class="form-control" id="_search" name="_search" placeholder="<?php print $text_enter_search_terms; ?>" />
              </div>
            </div>
            <div class="col<?php if(!FULL_GUI) { ?>-2<?php } ?> me-0 pe-0">
              <div class="d-flex align-items-center justify-content-start">
                <button id="button_search" class="btn btn-large btn-danger btn-search" onclick="Piler.auditexpert(this); return false;"><i class="bi bi-search icon-large"></i><?php if(FULL_GUI) { print '&nbsp;' . $text_search; } ?></button>
              </div>
            </div>
          </div>
        </div>
      </div>

    </div>

  <!-- Main content area with resizable panes -->

  <div class="container-fluid container-panes mt-2">
    <div class="pane pane-upper">

      <span id="qqq">Loading...</span>

    </div>

<?php if(FULL_GUI && PREVIEW_PANE) { ?>
    <div class="resizer"></div>

    <div id="preview" class="pane pane-lower">
      <h2>Preview Pane</h2>
      <p>Click on a message to show</p>
    </div>
  </div>
<?php } ?>

<?php print JS_CODE; ?>
<script src="<?php print PATH_PREFIX; ?>assets/js/resizer.js"></script>

</body>
</html>
