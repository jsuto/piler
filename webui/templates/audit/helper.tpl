<div id="resultslistcontainer" class="pane-upper-content">
  <table id="results" class="table table-striped mt-0">
    <thead class="table-secondary">
      <tr>
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
        <th class="auditcell ip header">
          <?php print $text_ipaddr; ?>
          <a class="navlink" xid="ipaddr" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up"></i></a>
          <a class="navlink" xid="ipaddr" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down"></i></a>
        </th>
        <th class="auditcell action header">
          <?php print $text_action; ?>
          <a class="navlink" xid="action" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up"></i></a>
          <a class="navlink" xid="action" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down"></i></a>
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
    <?php foreach ($messages as $message) { ?>
      <tr>
        <td class="auditcell date"><?php print $message['date']; ?></td>
        <td class="auditcell user"><?php print $message['email']; ?></td>
        <td class="auditcell ip"><?php print $message['ipaddr']; ?></td>
        <td class="auditcell action"><?php print $actions[$message['action']]; ?></td>
        <td class="auditcell description"><?php if($message['description'] != $message['shortdescription']) { ?><span title="<?php print $message['description']; ?>"><?php print $message['shortdescription']; ?></span><?php } else { print $message['description']; } ?></td>
        <td class="auditcell ref"><?php if(Registry::get('auditor_user') == 1 && $message['id'] > 0) { ?><a href="#" onclick="Piler.view_message(<?php print $message['id']; ?>);"><?php print $message['id']; ?></a><?php } ?></td>
      </tr>
    <?php } ?>
    </tbody>
  </table>
</div>


<div id="messagelistfooter" class="boxfooter upper-pane-fixed">
  <div id="pagingrow">
  <?php if($n > 0) { include_once DIR_BASE . 'templates/common/paging.tpl'; } ?>

  <?php if($n > 0) { ?>
    <a href="index.php?route=audit/download"><button class="btn btn-small btn-secondary">Export CSV</button></a>
  <?php } ?>

  </div>
</div>
