
<div id="messagelistcontainer" class="boxlistcontent">
    <?php if($n > 0) { ?>
    <table id="results" class="table table-striped table-condensed">
      <tbody>
    <?php $i=0; foreach ($messages as $message) { ?>
            
         <tr onmouseover="Piler.current_message_id = <?php print $message['id']; ?>; return false;" id="e_<?php print $message['id']; ?>" class="resultrow new <?php if($message['deleted'] == 1) { ?>xxx<?php } ?>" onclick="Piler.view_message_by_pos(<?php print $i; ?>);">
            <td id="c2_r<?php print $i; ?>" class="resultcell"><?php print ($page*$page_len) + $i + 1; ?>.</td>
            <td id="c3_r<?php print $i; ?>" class="resultcell"><?php print $message['date']; ?><br /><?php print $message['shortfrom']; ?><br /><?php print $message['shortsubject']; ?></td>
         </tr>

    <?php $i++; } ?>
      </tbody>
      
    </table>

    <?php } else if($n == 0) { ?>
                <div class="alert alert-block alert-error lead"><i class="glyphicon glyphicon-exclamation-sign icon-2x pull-left"></i> <?php print $text_empty_search_result; ?></div>
    <?php } ?>

</div>

<div id="messagelistfooter" class="boxfooter">
    <div class="row-fluid">
       <div id="pagingrow" class="span4">
            <div id="pagingbox">
    <?php if($n > 0){ ?>
            &nbsp;
            <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(0);"><i class="glyphicon glyphicon-chevron-left"></i></a><?php } else { ?><span class="navlink"><i class="glyphicon glyphicon-chevron-left muted"></i></span><?php } ?>
            &nbsp;
            <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $prev_page; ?>);"><i class="glyphicon glyphicon-menu-left"></i></a><?php } else { ?><span class="navlink"><i class="glyphicon glyphicon-menu-left"></i></span><?php } ?>
            &nbsp;

            <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $hits; ?><?php if($total_found > MAX_SEARCH_HITS) { ?> (<?php print $total_found; ?>)<?php } ?>

            &nbsp;
            <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $next_page; ?>);"><i class="glyphicon glyphicon-menu-right"></i></a> <?php } else { ?><span class="navlink"><i class="glyphicon glyphicon-menu-right muted"></i></span><?php } ?>
            &nbsp;
            <?php if($page < $total_pages) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $total_pages; ?>);"><i class="glyphicon glyphicon-chevron-right"></i></a><?php } else { ?> <span class="navlink"><i class="glyphicon glyphicon-chevron-right muted"></i></span><?php } ?>
            &nbsp;

    <?php } else { print $text_none_found; } ?>
            </div>
        </div>

    </div>
</div>
