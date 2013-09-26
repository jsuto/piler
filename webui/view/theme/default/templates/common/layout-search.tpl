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

    <link href="/view/theme/default/assets/css/metro-bootstrap.css" rel="stylesheet">

    <!-- HTML5 shim, for IE6-8 support of HTML5 elements -->
    <!-- original location: http://html5shim.googlecode.com/svn/trunk/html5.js -->
    <!--[if lt IE 9]>
      <script src="/view/theme/default/assets/js/html5.js"></script>
      <style>body{padding-top:70px;}</style>
    <![endif]-->
	
    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <?php if(BRANDING_FAVICON) { ?><link rel="shortcut icon" href="<?php print BRANDING_FAVICON; ?>" /><?php } ?>
    
    <script type="text/javascript" src="/view/javascript/jquery.min.js"></script>
    <script type="text/javascript" src="/view/javascript/jquery-ui-custom.min.js"></script>
    <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>
    <script type="text/javascript" src="/view/theme/default/assets/js/bootstrap.js"></script>
    <script type="text/javascript" src="/view/javascript/piler.js"></script>
</head>

<body onload="Piler.add_shortcuts();">

    <div id="menu">
        <?php print $menu; ?>
    </div>
    <div id="messagebox1" class="alert alert-info lead"></div>

    <div id="piler1" class="container-fluid">

    <div id="searchcontainer">
         <input type="hidden" name="searchtype" id="searchtype" value="expert" />
         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />
         <input type="hidden" name="prefix" id="prefix" value="" />

         <div class="control-group">
            <div class="controls row-fluid">
                <div id="input-span" class="span6">
                    <label for="_search">Search</label>
                <input type="text" id="_search" name="_search" placeholder="<?php print $text_enter_search_terms; ?>" />
                </div>
                <div class="span6 input-append btn-group">
                    <button id="button_search" class="btn btn-large btn-danger" onclick="Piler.expert(this);"><i class="icon-search icon-large"></i>&nbsp;<?php print $text_search; ?></button>
                    <button id="button_expert" class="btn btn-large btn-inverse" onclick="$('#searchpopup1').show();"><?php print $text_advanced_search; ?> &nbsp;<span class="caret"></span></button>
                    <button id="button_options" class="btn btn-large btn-inverse dropdown-toggle" data-toggle="dropdown"><?php print $text_options; ?> &nbsp;<span class="caret"></span></button>
                    <ul class="dropdown-menu">
                        <li><a href="#" onclick="Piler.saved_search_terms('<?php print $text_saved; ?>');"><?php print $text_save; ?></a></li>
                        <li><a href="#" onclick="Piler.load_saved_search_terms();"><?php print $text_load; ?></a></li>
                    </ul>
                </div>
              </div>
            </div>
         </div>
    </div>
    <?php print $popup; ?>
    <div id="mainscreen">
        <div id="mailleftcontainer">
        <?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
        <div id="mailboxlist-container">
        <?php print $folder; ?>
        </div>
        <script type="text/javascript">
          var mailviewsplitv = new rcube_splitter({id:'splitter1', p1: 'mailleftcontainer', p2: 'mailrightcontainer', orientation: 'v', relative: true, start: 165});
          split.add_onload('mailviewsplitv.init()');
        </script>
        <?php } ?>
        </div>
        <div id="mailrightcontainer<?php if(ENABLE_FOLDER_RESTRICTIONS == 0) { ?>nofolder<?php } ?>">

            <div id="mailrightcontent">
              <div id="mailcontframe">
                <div id="sspinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_working; ?></div>
                <div id="messagelistcontainer" class="boxlistcontent" style="top:0"> 
                
                <?php print $content; ?>

                </div>
              </div>

        <script type="text/javascript">
          var mailviewsplit = new rcube_splitter({id:'splitter2', p1: 'mailcontframe', p2: 'mailpreviewframe', orientation: 'h', relative: true, start: 341});
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
