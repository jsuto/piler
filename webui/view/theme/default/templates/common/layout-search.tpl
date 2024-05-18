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

   <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
   <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap-icons@1.11.1/font/bootstrap-icons.min.css" integrity="sha256-6MNujrdbV0Z7S927PzUMXOmMLwkKdsdD7XIl/w89HMQ=" crossorigin="anonymous">
   <link href="/view/theme/default/assets/css/1.css" rel="stylesheet">

   <!-- Fav and touch icons -->
   <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
   <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
   <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
   <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
   <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body onload="Piler.add_shortcuts(); Piler.expert();">

<?php include_once DIR_THEME . THEME . '/templates/common/common.tpl'; ?>
<?php include_once DIR_THEME . THEME . '/templates/search/modals.tpl'; ?>

<?php if( (OUTLOOK == 1 && SHOW_MENU_FOR_OUTLOOK == 1) || (OUTLOOK == 0 && MOBILE_DEVICE == 0) ) { ?>
  <div id="header">
    <?php print $menu; ?>
  </div>
<?php } ?>

    <div class="searchcontainer">
      <div class="row w-100 ms-0">

        <div class="container text-center">
          <div class="row align-items-center">
            <div class="col ps-0">
              <div class="d-flex align-items-center justify-content-start">
                <input type="hidden" name="searchtype" id="searchtype" value="expert" />
                <input type="hidden" name="sort" id="sort" value="date" />
                <input type="hidden" name="order" id="order" value="0" />
                <input type="hidden" name="ref" id="ref" value="" />
                <input type="hidden" name="prefix" id="prefix" value="" />
                <input type="text" class="form-control" id="_search" name="_search" placeholder="Enter your search terms" />
              </div>
            </div>
            <div class="col me-0 pe-0">
              <div class="d-flex align-items-center justify-content-start">
                <button id="button_search" class="btn btn-large btn-danger btn-search" onclick="Piler.expert(this); return false;"><i class="bi bi-search icon-large"></i>&nbsp;Search</button>
                <button id="button_expert" class="btn btn-large btn-secondary btn-search" data-bs-toggle="modal" data-bs-target="#advancedSearchModal">Advanced search</button>

                <button id="button_options" class="btn btn-large btn-secondary btn-options" type="button" data-bs-toggle="dropdown" aria-expanded="false">Options</button>
                  <ul class="dropdown-menu">
                    <li><a class="dropdown-item" href="#" onclick="Piler.saved_search_terms('Saved');">Save</a></li>
                    <li><a class="dropdown-item" href="#" onclick="Piler.load_saved_search_terms();">Load</a></li>
                  </ul>
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
    <div class="resizer"></div>

    <div id="preview" class="pane pane-lower">
      <h2>Preview Pane</h2>
      <p>Click on a message to show</p>
    </div>
  </div>

<?php print JS_CODE; ?>
<script src="/view/javascript/resizer.js"></script>

</body>
</html>
