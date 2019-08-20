
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
             <?php print $text_delete_reason; ?>
             <a xid="action" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="action" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th class="auditcell description header">
             <?php print $text_description; ?>
          </th>
          <th class="auditcell ref header">
             <?php print $text_ref; ?>
          </th>
          <th class="auditcell removal header">
             &nbsp;
          </th>
          <th class="auditcell cancel header">
             &nbsp;
          </th>
       </tr>
    </thead>

    <tbody>

<?php $i=0; foreach ($data as $d) { $i++; ?>
         <tr class="resultrow<?php if($i % 2) { ?> odd<?php } ?>">
            <td class="auditcell date"><?php print date(DATE_TEMPLATE . " H:i", $d['date1']); ?></td>
            <td class="auditcell user"><?php print $d['requestor']; ?></td>
            <td class="auditcell date"><?php print $d['reason1']; ?></td>
            <td class="auditcell ip"><?php print $d['reason2']; ?></td>
            <td class="auditcell ref"><?php if($d['deleted'] == -1) { ?><a href="#" onclick="Piler.view_message(<?php print $d['id']; ?>);"><?php } print $d['id']; if($d['deleted'] == 0) { ?></a><?php } ?></td>
            <td class="auditcell aaa"><a href="#" data-toggle="modal" data-target="#delete-approved-modal" data-id="<?php print $d['id']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
            <td class="auditcell aaa"><a href="#" data-toggle="modal" data-target="#delete-rejected-modal" data-id="<?php print $d['id']; ?>"><i class="icon-ban-circle"></i>&nbsp;<?php print $text_cancel; ?></a></td>

         </tr>
<?php } ?>

    </tbody>
   </table>

