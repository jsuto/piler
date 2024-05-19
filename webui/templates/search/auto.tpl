<!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="utf-8">
    <title><?php print $title; ?></title>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <?php if(SITE_KEYWORDS) { ?><meta name="keywords" content="<?php print SITE_KEYWORDS; ?>" /><?php } ?>
    <?php if(SITE_DESCRIPTION) { ?><meta name="description" content="<?php print SITE_DESCRIPTION; ?>" /><?php } ?>
    <?php if(PROVIDED_BY) { ?><meta name="author" content="<?php print PROVIDED_BY; ?>" /><?php } ?>
  </head>

<body>

    <p style="font-weight: bold; font-size: 18px;"><?php print $text_search_expression; ?>: <?php print $search_expression; ?><br />
    <?php if($a['date1']) { print $text_date; ?>: <?php print $a['date1']; } ?></p>

    <?php if($n > 0) { ?>
    <table id="results" class="table table-striped table-condensed">
      <thead>
        <tr>
          <th align="left" id="id-header">&nbsp;</th>
          <th align="left" id="date-header">
             <?php print $text_date; ?>
          </th>
          <th align="left" id="from-header">
             <?php print $text_from; ?>
          </th>
          <th align="left" id="to-header">
             <?php print $text_to; ?>
          </th>
          <th align="left" id="subject-header">
             <?php print $text_subject; ?>
          </th>
          <th align="left" id="size-header">
             <?php print $text_size; ?>
          </th>
          <th align="left" id="spam-header"><?php print $text_spam; ?></th>
          <th align="left" id="attachment-header"><?php print $text_attachment; ?></th>
       </tr>
      </thead>
      <tbody>
    <?php $i=0; foreach ($messages as $message) { ?>

         <tr id="e_<?php print $message['id']; ?>" class="resultrow new">
            <td id="c2_r<?php print $i; ?>" class="resultcell id"><?php print ($page*$page_len) + $i + 1; ?>.</td>
            <td id="c3_r<?php print $i; ?>" class="resultcell date"><?php print $message['date']; ?></td>
            <td id="c4_r<?php print $i; ?>" class="resultcell from"><?php if($message['from'] != $message['shortfrom']) { ?><span title="<?php print $message['from']; ?>"><?php print $message['shortfrom']; ?></span><?php } else { print $message['from']; } ?></td>
            <td id="c5_r<?php print $i; ?>" class="resultcell to"><?php if($message['to'] != $message['shortto']) { ?><span title="<?php print $message['to']; ?>"><?php print $message['shortto']; ?>&nbsp;<i class=" muted icon-group"></i></span><?php } else { print $message['to']; } ?></td>

            <td id="c6_r<?php print $i; ?>" class="resultcell subject"><a href="<?php print SITE_URL; ?>message.php/a<?php print $message['id']; ?>"><?php print $message['subject']; ?></a></td>

            <td id="c7_r<?php print $i; ?>" class="resultcell size"><?php print $message['size']; ?></td>
            <td align="center" id="c8_r<?php print $i; ?>" class="resultcell end"><?php if($message['spam'] == 1) { ?>!<?php } else { ?>&nbsp;<?php } ?></td>
            <td align="center" id="c9_r<?php print $i; ?>" class="resultcell end"><?php if($message['attachments'] > 0) { ?>+<?php } else { ?>&nbsp;<?php } ?></td>
         </tr>

    <?php $i++; } ?>
      </tbody>

    </table>

    <?php } else if($n == 0) { ?>
                <div class="alert alert-block alert-error lead"><i class="icon-exclamation-sign icon-2x pull-left"></i> <?php print $text_empty_search_result; ?></div>
    <?php } ?>


</body>
</html>
