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
  <?php print CUSTOM_CSS; ?>

  <?php print BRANDING_FAVICON; ?>

  <script type="text/javascript">

    function append_value_from_slider(id, value)
    {
        var prefix = '\n';

        a = opener.document.getElementById(id);
        if(a && a.value == '') prefix = '';

        a.value += prefix + value;
    }

  </script>

</head>

<body class="email">

<div id="wrapemail">

<?php if(isset($title) && $title) { ?><h3><?php print $title; ?></h3><?php } ?>

<?php print $content; ?>

</div> <!-- wrap -->

<?php if(TRACKING_CODE) { print TRACKING_CODE; } ?>

</body>
</html>
