<div id="deleteconfirm-modal" class="modal hide fade">
  <div class="modal-header">
    <button type="button" class="close" data-dismiss="modal" role="dialog" aria-hidden="true"><i class="icon-remove"></i></button>
    <h3><?php print $text_confirm; ?> <?php print $text_delete; ?></h3>
  </div>
  <div class="modal-body">
    <p>Remove message <span id="name">ERROR</span>?</p>
  </div>
  <div class="modal-footer">
    <a href="#" class="btn" data-dismiss="modal" aria-hidden="true"><?php print $text_close; ?></a>
    <a href="index.php?route=message/remove&amp;id=Error&amp;confirmed=0" class="btn btn-primary" id="id"><?php print $text_delete; ?></a>
  </div>
</div>

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
          <th class="auditcell removal header">
             &nbsp;
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
            <td class="auditcell aaa"><a href="#" class="confirm-delete" data-id="<?php print $d['id']; ?>" data-name="<?php print $d['id']; ?>"><i class="icon-remove-sign"></i>&nbsp;<?php print $text_remove; ?></a></td>
         </tr>
<?php } ?>

    </tbody>
   </table>

