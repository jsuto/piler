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
          <th class="auditcell action header">
             <?php print $text_delete; ?>
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

<?php $i=0; foreach ($data as $d) { $i++; ?>
         <tr class="resultrow<?php if($i % 2) { ?> odd<?php } ?>">
            <td class="auditcell date"><?php print date(DATE_TEMPLATE . " H:i", $d['date1']); ?></td>
            <td class="auditcell user"><?php print $d['email']; ?></td>
            <td class="auditcell date"><?php print $d['deleted']; ?></td>
            <td class="auditcell ip"><?php print $d['reason']; ?></td>
            <td class="auditcell ref"><?php if($d['deleted'] == 0) { ?><a href="#" onclick="Piler.view_message(<?php print $d['id']; ?>);"><?php } print $d['id']; if($d['deleted'] == 0) { ?></a><?php } ?></td>
         </tr>
<?php } ?>

    </tbody>
   </table>

</div>


<!--div id="messagelistfooter" class="boxfooter">
</div-->
