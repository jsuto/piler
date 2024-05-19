
   <table id="resultstable" class="table table-striped table-condensed">
    <thead id="resultstop">
       <tr class="resultrow">
          <th class="auditcell date header">
             <?php print $text_date; ?>
             <a class="navlink" xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up"></i></a>
             <a class="navlink" xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down"></i></a>
          </th>
          <th class="auditcell user header">
             <?php print $text_user; ?>
             <a class="navlink" xid="user" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up"></i></a>
             <a class="navlink" xid="user" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down"></i></a>
          </th>
          <th class="auditcell action header">
             <?php print $text_delete_reason; ?>
             <a class="navlink" xid="action" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up"></i></a>
             <a class="navlink" xid="action" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down"></i></a>
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
            <td class="auditcell aaa"><a href="#" onclick="Piler.modal('removeApproveModal', true); Piler.remove_message_id=<?php print $d['id']; ?>;"><i class="bi bi-trash text-danger" title="<?php print $text_remove; ?>"></i></a></td>
            <td class="auditcell aaa"><a href="#" onclick="Piler.modal('removeRejectModal', true); Piler.remove_message_id=<?php print $d['id']; ?>;"><i class="bi bi-ban" title="<?php print $text_cancel; ?>"></i></a></td>

         </tr>
<?php } ?>

    </tbody>
   </table>
