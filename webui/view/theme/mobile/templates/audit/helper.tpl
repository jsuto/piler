
<div id="messagelistcontainer" class="boxlistcontent">

   <div id="results">

<?php if($n > 0) { $i=0; foreach ($messages as $message) { $i++; ?>

         <div class="resultrow<?php if($i % 2) { ?> odd<?php } ?>">
            <div class="auditcell date"><?php print $message['date']; ?></div>
            <div class="auditcell user"><?php print $message['email']; ?></div>
            <div class="auditcell ip"><?php print $message['ipaddr']; ?></div>
            <div class="auditcell action"><?php print $actions[$message['action']]; ?></div>
            <div class="auditcell description"><?php if($message['description'] != $message['shortdescription']) { ?><span title="<?php print $message['description']; ?>"><?php print $message['shortdescription']; ?></span><?php } else { print $message['description']; } ?></div>
            <div class="auditcell ref"><?php if(Registry::get('auditor_user') == 1 && $message['id'] > 0) { ?><a href="#" class="auditlink" onclick="Piler.view_message(<?php print $message['id']; ?>);"><?php print $message['id']; ?></a><?php } ?></div>
         </div>

<?php } } else { ?>
    <div class="alert">
       <strong><?php print $text_empty_search_result; ?></strong>
    </div>

<?php } ?>

   </div>




<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(0);">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $prev_page; ?>);"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $next_page; ?>);">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="Piler.navigation(<?php print $total_pages; ?>);"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>

<?php } else { ?>&nbsp;<?php } ?>
   </div>
</div>


</div>

