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
    <div class="row w-100 ps-0 ms-0">

      <div class="container text-center">
        <div class="row align-items-center">
          <div class="col ps-0">
            <div class="d-flex align-items-center justify-content-start">
              <input type="hidden" name="searchtype" id="searchtype" value="expert" />
              <input type="hidden" name="sort" id="sort" value="date" />
              <input type="hidden" name="order" id="order" value="0" />
              <input type="hidden" name="ref" id="ref" value="" />
              <input type="hidden" name="prefix" id="prefix" value="" />
              <input type="text" class="form-control" id="_search" name="_search" placeholder="<?php print $text_enter_search_terms; ?>" />
            </div>
          </div>
          <div class="col me-0 pe-0">
            <div class="d-flex align-items-center justify-content-start">
              <button id="button_search" class="btn btn-large btn-danger btn-search" onclick="Piler.expert(this); return false;"><i class="bi bi-search"></i>&nbsp;<?php print $text_search; ?></button>
              <button id="button_expert" class="btn btn-large btn-secondary btn-search" data-bs-toggle="modal" data-bs-target="#advancedSearchModal"><?php print $text_advanced_search; ?></button>

              <button id="button_options" class="btn btn-large btn-secondary dropdown-toggle btn-options" type="button" data-bs-toggle="dropdown" aria-expanded="false"><?php print $text_options; ?></button>
                <ul class="dropdown-menu dropdown-menu-end">
                  <li><a class="dropdown-item" href="#" onclick="Piler.saved_search_terms('Saved');"><?php print $text_save; ?></a></li>
                  <li><a class="dropdown-item" href="#" onclick="Piler.load_saved_search_terms();"><?php print $text_load; ?></a></li>
                </ul>
            </div>
          </div>
        </div>
      </div>
    </div>

  </div>

  <!-- Main content area with resizable panes -->

  <div class="container-fluid container-panes mt-3">
    <div class="pane pane-upper">

      <span id="qqq"><?php print $text_loading; ?>...</span>

    </div>
    <div class="resizer"></div>

    <div id="preview" class="pane pane-lower">
      <h2>Preview Pane</h2>
      <p>Click on a message to show</p>
    </div>
  </div>

<?php print JS_CODE; ?>
<script src="<?php print PATH_PREFIX; ?>assets/js/resizer.js"></script>

</body>
</html>
