<div id="sspinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_working; ?></div>
<div id="resultscontainer"  class="boxlistcontent<?php if($n <= 0) { ?> empty<?php } ?>" >

   <table id="resultstable" class="table table-striped table-condensed">
    <thead id="resultstop">
       <tr class="resultrow">
          <th class="auditcell date header">
             <?php print $text_date; ?>
             <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell user header">
             <?php print $text_user; ?>
             <a xid="user" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="user" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell ip header">
             <?php print $text_ipaddr; ?>
             <a xid="ipaddr" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="ipaddr" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell action header">
             <?php print $text_action; ?>
             <a xid="action" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="action" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell description header">
             <?php print $text_description; ?>
          </th>
          <th class="auditcell ref header">
             <?php print $text_ref; ?>
          </th>

       </tr>
     </thead>
   
    <tbody>
<?php if($n > 0) { $i=0; foreach ($messages as $message) { $i++; ?>

         <tr class="resultrow<?php if($i % 2) { ?> odd<?php } ?>">
            <td class="auditcell date"><?php print $message['date']; ?></td>
            <td class="auditcell user"><?php print $message['email']; ?></td>
            <td class="auditcell ip"><?php print $message['ipaddr']; ?></td>
            <td class="auditcell action"><?php print $actions[$message['action']]; ?></td>
            <td class="auditcell description"><?php if($message['description'] != $message['shortdescription']) { ?><span title="<?php print $message['description']; ?>"><?php print $message['shortdescription']; ?></span><?php } else { print $message['description']; } ?></td>
            <td class="auditcell ref"><?php print $message['id']; ?></td>
         </tr>

<?php } } else { ?>
   <div class="alert alert-block alert-error lead"><i class="icon-exclamation-sign icon-2x pull-left"></i> <?php print $text_empty_search_result; ?></div>
<?php } ?>
    </tbody>
      <tfoot>
       <tr class="resultrow">
          <th class="auditcell date header">
             <?php print $text_date; ?>
             <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell user header">
             <?php print $text_user; ?>
             <a xid="user" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="user" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell ip header">
             <?php print $text_ipaddr; ?>
             <a xid="ipaddr" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="ipaddr" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell action header">
             <?php print $text_action; ?>
             <a xid="action" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="action" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell description header">
             <?php print $text_description; ?>
          </th>
          <th class="auditcell ref header">
             <?php print $text_ref; ?>
          </th>

       </tr>
     </tfoot>
   </table>

</div>


<div id="messagelistfooter" class="boxfooter">
    <div id="pagingrow" class="pull-left">
    <?php if($n >= $page_len){ ?>
            &nbsp;
            <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(0);"><i class="icon-double-angle-left icon-large"></i></a><?php } else { ?><span class="navlink"><i class="icon-double-angle-left icon-large"></i></span><?php } ?>
            &nbsp;
            <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $prev_page; ?>);"><i class="icon-angle-left icon-large"></i></a><?php } else { ?><span class="navlink"><i class="icon-angle-left icon-large"></i></span><?php } ?>
            &nbsp;
            <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
            &nbsp;
            <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $next_page; ?>);"><i class="icon-angle-right icon-large"></i></a><?php } else { ?><span class="navlink"><i class="icon-angle-right icon-large"></i></span><?php } ?>
            &nbsp;
            <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="Piler.navigation(<?php print $total_pages; ?>);"><i class="icon-double-angle-right icon-large"></i></a><?php } else { ?> <span class="navlink"><i class="icon-double-angle-right icon-large"></i></span><?php } ?>
            &nbsp;
    <?php } else { print $text_none_found; } ?>
    </div>
</div>


