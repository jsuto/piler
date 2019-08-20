<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print DEFAULT_LANG; ?>" lang="<?php print DEFAULT_LANG; ?>">

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

    <!-- HTML5 shim, for IE6-8 support of HTML5 elements -->
    <!-- original location: http://html5shim.googlecode.com/svn/trunk/html5.js -->
    <!--[if lt IE 9]>
      <script src="/view/theme/default/assets/js/html5.js"></script>
      <style>body{padding-top:70px;}</style>
    <![endif]-->

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

<div class="modal fade" id="exampleModal" tabindex="-1" role="dialog" aria-labelledby="exampleModalLabel">
  <div class="modal-dialog" role="document">
    <div class="modal-content">
      <div class="modal-header">
        <button type="button" class="close" data-dismiss="modal" aria-label="Close"><span aria-hidden="true">&times;</span></button>
        <h3 class="modal-title" id="exampleModalLabel"><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
      </div>
      <div class="modal-body">
        <p>Remove message <span id="id1">AAA</span>?</p>
      </div>
      <div class="modal-footer">
        <button type="button" class="btn btn-default" data-dismiss="modal">Close</button>
        <a href="#" class="btn btn-primary" id="delete-id"><?php print $text_delete; ?></a>
      </div>
    </div>
  </div>
</div>


    <div id="piler1" class="container">

      <div id="searchcontainer">

         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />

         <div class="control-group">
            <div class="controls row-fluid">
                <div id="input-span" class="span8">
                    <label for="_search">Search</label>
                    <input type="text" id="_search" name="_search" placeholder="<?php print $text_enter_search_terms; ?>" />
                </div>
                <div class="span4 input-append">
                    <button id="button_search" class="btn btn-large btn-danger" onclick="Piler.auditexpert(this);"><i class="icon-search icon-large"></i>&nbsp;<?php print $text_search; ?></button>
                </div>
            </div>
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

   $('#exampleModal').on('show.bs.modal', function (event) {
      var button = $(event.relatedTarget) // Button that triggered the modal
      var id = button.data('id')
      $('#delete-id').attr('href', 'index.php?route=message/remove&amp;id=' + id + '&amp;confirmed=1')
      $('#id1').text(id)
      var modal = $(this)
   })
</script>

<?php } ?>

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
