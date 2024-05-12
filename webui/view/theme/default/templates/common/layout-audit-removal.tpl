<!DOCTYPE html>
<html lang="<?php print DEFAULT_LANG; ?>">

<head>
    <title><?php print $title; ?></title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="<?php print DEFAULT_LANG; ?>" />
    <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
    <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
    <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
    <meta name="rating" content="general" />
    <meta name="robots" content="all" />

    <?php print CSS_CODE; ?>
    <?php print JS_CODE; ?>

    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body onload="Piler.add_shortcuts();">

  <!-- remove approval modal -->

  <div id="removeApproveModal" class="modal" tabindex="-1">
    <div class="modal-dialog modal-lg">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title">Remove message?</h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_cancel; ?></button>
          <button type="button" class="btn btn-primary" id="id2" onclick="Piler.approve_message_removal(Piler.remove_message_id);"><?php print $text_delete; ?></a></button>
        </div>
      </div>
    </div>
  </div>

  <!-- remove reject modal -->

  <div id="removeRejectModal" class="modal" tabindex="-1">
    <div class="modal-dialog modal-lg">
      <div class="modal-content">
        <div class="modal-header">
          <h5 class="modal-title"><?php print $text_reject; ?> <?php print $text_remove_message; ?></h5>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>
        <div class="modal-body mb-3">
          <label for="FormControlInput1" class="form-label"><?php print $text_reason_of_rejection; ?></label>
          <input type="text" class="form-control" id="reason2" name="reason2" />
        </div>
        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal"><?php print $text_cancel; ?></button>
          <button type="button" class="btn btn-primary" onclick="Piler.reject_message_removal(Piler.remove_message_id, $('#reason2').val());"><?php print $text_reject; ?></button>
        </div>
      </div>
    </div>
  </div>



    <div id="menu">
        <?php print $menu; ?>
    </div>

    <div id="messagebox1" class="audit audit-info"></div>


    <div id="mainscreen">

      <div id="mailleftcontainer">
      </div>

      <div id="mailrightcontainernofolder">

         <div id="mailrightcontent">

            <div id="mailcontframe">
               <div id="sspinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_working; ?></div>
               <div id="resultscontainer" class="boxlistcontent">
                  <?php print $content; ?>
               </div>
            </div>

<?php if(Registry::get('auditor_user') == 1 || Registry::get('data_officer') == 1) { ?>

            <script type="text/javascript">
               var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: 341});
               split.add_onload('mailviewsplit.init()');
            </script>

            <div id="mailpreviewframe"></div>

<?php } ?>

         </div>

      </div>

    </div>

<?php if(Registry::get('auditor_user') == 1 || Registry::get('data_officer') == 1) { ?>

<script type="text/javascript">
   $(document).ready(function(){
      split.init();
   });
</script>

<?php } ?>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
