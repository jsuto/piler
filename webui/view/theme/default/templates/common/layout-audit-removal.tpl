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

    <link href="/view/theme/default/assets/css/metro-bootstrap.css" rel="stylesheet" />

    <?php print JS_CODE; ?>

    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
</head>

<body onload="Piler.add_shortcuts();">

    <div id="menu">
        <?php print $menu; ?>
    </div>

    <div id="messagebox1" class="audit audit-info"></div>

    <div id="delete-approved-modal" class="modal fade" tabindex="-1" role="dialog" aria-labelledby="deleteApprovedLabel">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
        <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
      </div>
      <div class="modal-body">
        <p>Remove message <span id="id1"/>?</p>
      </div>
      <div class="modal-footer">
        <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
        <a href="#" class="btn btn-primary" id="id2"><?php print $text_delete; ?></a>
      </div>
    </div>

    <div id="delete-rejected-modal" class="modal hide fade">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
        <h3><?php print $text_reject; ?> <?php print $text_remove_message; ?> <span id="reject_id1"/></h3>
      </div>
      <div class="modal-body">
        <form id="reject_form1">
          <div class="form-group">
            <label for="message-text" class="control-label" id="reject_label"><?php print $text_reason_of_rejection; ?>:</label>
            <textarea class="form-control" id="reason2"></textarea>
          </div>
        </form>
      </div>
      <div class="modal-footer">
        <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
        <a href="#" class="btn btn-primary" id="reject_id2"><?php print $text_reject; ?></a>
      </div>
    </div>

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

   $('#delete-approved-modal').on('show.bs.modal', function (event) {
      var button = $(event.relatedTarget)
      var id = button.data('id')
      $('#id1').text(id)
      $('#id2').attr('href', 'index.php?route=message/remove&id=' + id + '&confirmed=1')
   })

   var reject_id
   var reject_label_color
   var reason2_border

   $('#delete-rejected-modal').on('show.bs.modal', function (event) {
      $('#reason2').focus()

      var button = $(event.relatedTarget)
      reject_id = button.data('id')

      $('#reject_id1').text(reject_id)
   })

   $('#delete-rejected-modal').on('hide.bs.modal', function (event) {
      $('#reject_label').css({'color':reject_label_color})
      $('#reason2').css({'border':reason2_border})
   })

   $('#reject_id2').on('click', function() {
      var reason2 = $.trim($('#reason2').val());
      if(reason2) {
         Piler.reject_removing_message(reject_id, reason2)
      } else {
         reject_label_color = $('#reject_label').css('color')
         reason2_border = $('#reason2').css('border')

         $('#reject_label').css({'color':'red'})
         $('#reason2').css({'border':'1px solid red'})
      }
   });

</script>

<?php } ?>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
