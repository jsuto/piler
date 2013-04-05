<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="hu" lang="hu">

<head>
    <title><?php print $title; ?> | <?php print SITE_NAME; ?> | <?php print PROVIDED_BY; ?></title>
    <meta http-equiv="content-type" content="text/html; charset=utf-8" />
    <meta http-equiv="Content-Language" content="en" />
    <meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" />
    <meta name="description" content="<?php print SITE_DESCRIPTION; ?>" />
    <meta name="author" content="<?php print PROVIDED_BY; ?>" />
    <meta name="rating" content="general" />
    <meta name="robots" content="all" />

    <link href="/view/theme/default/assets/css/metro-bootstrap.css" rel="stylesheet" />
    
    <!-- HTML5 shim, for IE6-8 support of HTML5 elements -->
    <!-- original location: http://html5shim.googlecode.com/svn/trunk/html5.js -->
    <!--[if lt IE 9]>
      <script src="/view/theme/default/assets/js/html5.js"></script>
    <![endif]-->
    
    <script type="text/javascript" src="/view/javascript/jquery.min.js"></script>
    <script type="text/javascript" src="/view/javascript/jquery-ui-custom.min.js"></script>
    <script type="text/javascript" src="/view/javascript/rc-splitter.js"></script>
    <script type="text/javascript" src="/view/theme/default/assets/js/bootstrap.js"></script>
    <script type="text/javascript" src="/view/javascript/piler.js"></script>
    <!-- Fav and touch icons -->
    <link rel="apple-touch-icon-precomposed" sizes="144x144" href="/view/theme/default/assets/ico/apple-touch-icon-144-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="114x114" href="/view/theme/default/assets/ico/apple-touch-icon-114-precomposed.png">
    <link rel="apple-touch-icon-precomposed" sizes="72x72" href="/view/theme/default/assets/ico/apple-touch-icon-72-precomposed.png">
    <link rel="apple-touch-icon-precomposed" href="/view/theme/default/assets/ico/apple-touch-icon-57-precomposed.png">
    <link rel="shortcut icon" href="/view/theme/default/assets/ico/favicon.png">
</head>

<body onload="Piler.add_shortcuts();">

<div id="messagebox1" class="audit audit-info"></div>

<div id="piler1" class="container">

<div id="menu"><?php print $menu; ?></div>

<div id="searchcontainer">

    <input type="hidden" name="searchtype" id="searchtype" value="expert" />
    <input type="hidden" name="sort" id="sort" value="date" />
    <input type="hidden" name="order" id="order" value="0" />

    <div class="control-group">
        <div class="controls">
            <label for="_search">Search</label>
            <div class="input-append">            
                <input type="text" id="_search" name="_search" placeholder="<?php print $text_enter_search_terms; ?>" />
                <button id="button_search" class="btn btn-large btn-danger" onclick="Piler.auditexpert(this);"><i class="icon-search icon-large"></i>&nbsp;<?php print $text_search; ?></button>
            </div>
        </div>
    </div>
</div>

<div id="mainscreen">

  <div id="mailleftcontainer">
  </div>

  <div id="mailrightcontainer<?php if(ENABLE_FOLDER_RESTRICTIONS == 0) { ?>nofolder<?php } ?>">

    <div id="mailrightcontent">

      <div id="mailcontframe">
        <div id="sspinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_working; ?></div>
        <div id="resultscontainer" class="boxlistcontent"> 

              <?php print $content; ?>

        </div>
      </div>

  </div>

</div>

</div>

</body>
</html>
