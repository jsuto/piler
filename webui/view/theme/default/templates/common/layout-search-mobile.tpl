<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print DEFAULT_LANG; ?>" lang="<?php print DEFAULT_LANG; ?>">

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

   <link rel="stylesheet" href="https://stackpath.bootstrapcdn.com/bootstrap/3.4.1/css/bootstrap.min.css" integrity="sha384-HSMxcRTRxnN+Bdg0JdbxYKrThecOKuH5zCYotlSAcp1+c8xmyTe9GYg1l9a69psu"
 crossorigin="anonymous">
   <link rel="stylesheet" href="/view/theme/default/assets/css/piler.css" />

   <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>

   <?php print JS_CODE; ?>
   <?php print CSS_CODE; ?>
</head>

<body onload="Piler.add_shortcuts();">

<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="glyphicon glyphicon-remove"></i></button>
    <h3><?php print $text_forward_selected_emails_to; ?></h3>
  </div>
  <div class="modal-body">
    <input type="text" id="restore_address" name="restore_address" />
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="#" onclick="var addr =  $('#restore_address').val(); if(addr) { Piler.bulk_restore_messages('<?php print $text_restored; ?>', addr); }" class="btn btn-primary" data-dismiss="modal" aria-hidden="true">OK</a>
  </div>
</div>

   <div id="piler1" class="container-fluid">
         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />
         <input type="hidden" name="prefix" id="prefix" value="" />

      <div class="row">
         <div class="col-xs-8"><input type="text" id="_search" name="_search" class="form-control" placeholder="Search" /></div>
         <div class="col-xs-4"><button id="button_search" class="btn btn-default btn-danger" onclick="Piler.expert(this); return false;"><span class="glyphicon glyphicon-search" aria-hidden="true"></span></button></div>
      </div>
   </div>


    <div id="mainscreen">
        <div id="mailleftcontainer">
        </div>
        <div id="mailrightcontainer">

            <div id="mailrightcontent">
              <div id="mailcontframe">
                <div id="messagelistcontainer" class="boxlistcontent top0">
                <?php print $content; ?>
                </div>
              </div>

        <script type="text/javascript">
          var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: (document.getElementById('mainscreen').clientHeight / 2)});
          split.add_onload('mailviewsplit.init()');
        </script>

              <div id="mailpreviewframe"></div>

          </div>

        </div>
    </div>

    <script type="text/javascript">
    $(document).ready(function(){
       split.init();
    });
    </script>


<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
