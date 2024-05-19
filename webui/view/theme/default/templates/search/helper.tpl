<div id="messagelistcontainer" class="pane-upper-content">
  <table id="results" class="table table-striped mt-0">
    <thead class="table-secondary">
      <tr>
        <th id="restore-header"><input type="checkbox" id="bulkcheck" name="bulkcheck" value="1" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" onclick="Piler.toggle_bulk_check('');" /></th>
        <th id="id-header">&nbsp;</th>
        <th id="date-header">
             <?php print $text_date; ?>
             <a class="navlink" xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up<?php if($sort == 'date' && $order == 1) { ?> bottomborder<?php } ?>"></i></a>
             <a class="navlink" xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down<?php if($sort == 'date' && $order == 0) { ?> bottomborder<?php } ?>"></i></a>
        </th>
        <th id="from-header">
             <?php print $text_from; ?>
             <a class="navlink" xid="from" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up<?php if($sort == 'from' && $order == 1) { ?> bottomborder<?php } ?>"></i></a>
             <a class="navlink" xid="from" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down<?php if($sort == 'from' && $order == 0) { ?> bottomborder<?php } ?>"></i></a>
        </th>
        <th id="to-header">
             <?php print $text_to; ?>
             <a class="navlink" xid="to" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up"></i></a>
             <a class="navlink" xid="to" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down"></i></a>
        </th>
        <th id="subject-header">
             <?php print $text_subject; ?>
             <a class="navlink" xid="subj" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up<?php if($sort == 'subj' && $order == 1) { ?> bottomborder<?php } ?>"></i></a>
             <a class="navlink" xid="subj" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down<?php if($sort == 'subj' && $order == 0) { ?> bottomborder<?php } ?>"></i></a>
        </th>
        <th id="size-header">
             <?php print $text_size; ?>
             <a class="navlink" xid="size" xorder="1" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-up<?php if($sort == 'size' && $order == 1) { ?> bottomborder<?php } ?>"></i></a>
             <a class="navlink" xid="size" xorder="0" onclick="Piler.changeOrder(this);"><i class="bi bi-chevron-down<?php if($sort == 'size' && $order == 0) { ?> bottomborder<?php } ?>"></i></a>
        </th>
        <th id="spam-header"><i class="bi bi-exclamation-triangle spam" title="<?php print $text_spam_flag; ?>"></i></th>
        <th id="attachment-header"><i class="bi bi-paperclip attachment" title="<?php print $text_attachment_flag; ?>"></i></th>
        <th id="note-header"><i class="bi bi-sticky notes" title="<?php print $text_notes_flag; ?>"></i></th>
        <th id="tag-header"><i class="bi bi-tag tag" title="<?php print $text_tag_flag; ?>"></i>
     <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
        <th id="verify-header"><i class="verified bi bi-check-circle-fill text-success" title="<?php print $text_verified_flag; ?>"></i></th>
     <?php } ?>
      </tr>
    </thead>
    <tbody>

    <?php $i=0; foreach ($messages as $message) { ?>

         <tr onmouseover="Piler.current_message_id = <?php print $message['id']; ?>; return false;" id="e_<?php print $message['id']; ?>" class="resultrow new <?php if($message['deleted'] == 1) { ?>xxx<?php } ?>" onclick="Piler.view_message_by_pos(<?php print $i; ?>);">
            <td id="c1_r<?php print $i; ?>" class="resultcell restore" onclick="Piler.stop_propagation(event);"><input type="checkbox" id="r_<?php print $message['id']; ?>" name="r_<?php print $message['id']; ?>" value="iiii" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" /></td>
            <td id="c2_r<?php print $i; ?>" class="resultcell id"><?php print ($page*$page_len) + $i + 1; ?></td>
            <td id="c3_r<?php print $i; ?>" class="resultcell date"><?php print $message['date']; ?></td>
            <td id="c4_r<?php print $i; ?>" class="resultcell from"><?php if($message['from'] != $message['shortfrom']) { ?><span title="<?php print $message['from']; ?>"><?php print $message['shortfrom']; ?></span><?php } else { print $message['from']; } ?></td>
            <td id="c5_r<?php print $i; ?>" class="resultcell to"><?php if(count($message['to']) > 1) { ?><span title="<?php print implode("\n", $message['to']); ?>"><?php print $message['shortto']; ?>&nbsp;<i class="muted icon-group"></i></span><?php } else { print $message['shortto']; } ?></td>

            <td id="c6_r<?php print $i; ?>" class="resultcell subject"><a href="#" <?php if($message['deleted'] == 1) { ?>class="xxx"<?php } ?>><?php print $message['subject']; ?></a><?php if(ENABLE_REFERENCES == 1 && $message['reference']) { ?> <a href="#" <?php if($message['deleted'] == 1) { ?>class="xxx"<?php } ?> title="<?php print $text_conversation_available; ?>" onclick="$('#ref').val('<?php print $message['reference']; ?>'); Piler.expert(this);">[+]</span></a><?php } ?><?php if($message['private'] == 1) { ?> <i class="bi bi-incognito private" title="private"></i><?php } ?> <?php if($message['marked_for_removal'] == 1) { ?> <span class="private">R</span><?php } ?></td>

            <td id="c7_r<?php print $i; ?>" class="resultcell size"><?php print $message['size']; ?></td>
            <td id="c8_r<?php print $i; ?>" class="resultcell end"><?php if($message['spam'] == 1) { ?><i class="bi bi-exclamation-triangle spam" title="<?php print $text_spam_flag; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
            <td id="c9_r<?php print $i; ?>" class="resultcell end"><?php if($message['attachments'] > 0) { ?><i class="bi bi-paperclip attachment" title="<?php print $text_attachment_flag; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
            <td id="c10_r<?php print $i; ?>" class="resultcell end"><?php if($message['note']) { ?><i class="bi bi-sticky notes" title="<?php print $message['note']; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
            <td id="c11_r<?php print $i; ?>" class="resultcell end"><?php if($message['tag']) { ?><i class="bi bi-tag tag" title="<?php print $message['tag']; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { if ($message['verification'] == 1) {?>
            <td id="c12_r<?php print $i; ?>" class="resultcell end"><i class="verified bi bi-check-circle-fill text-success" title="<?php print $text_verified_flag; ?>"></i></td>
         <?php } else { ?>
            <td id="c12_r<?php print $i; ?>" class="resultcell end"><i class="unverified bi bi-x-square-fill text-danger" title="<?php print $text_unverified_flag; ?>"></i></td>
         <?php } } ?>
         </tr>

    <?php $i++; } ?>
      </tbody>

  </table>

</div>

<div id="messagelistfooter" class="boxfooter row w-100 ms-0 upper-pane-fixed">
        <div class="col ps-0">
          <div class="d-flex align-items-center justify-content-start functionbox ps-0 pt-1 pb-0 mb-0">

    <?php if($n > 0) {
         include_once DIR_THEME . THEME . '/templates/common/paging.tpl';

         if(Registry::get('auditor_user') == 1 && $session->get("sphx_query")) { ?>
            <span class="ms-5"><a href="#" onclick="Piler.show_message('messagebox1', '<?php H($session->get("sphx_query")); ?>', 5);">sphinx</a></span>
         <?php } ?>

    <?php } else { ?><span class="text-danger fs-3"><?php print $text_none_found; ?></span><?php } ?>


          </div>

        </div>

        <div class="col pe-0">
          <div class="d-flex align-items-center justify-content-end functionbox pt-1 pb-0 mb-0">

            <input type="hidden" id="tag_keys" name="tag_keys" value="<?php print $all_ids; ?>" />
            <input type="hidden" id="_ref" name="_ref" value="<?php if(isset($_ref)) { print $_ref; } ?>" />

                <span class=""><?php print $text_with_selected; ?>:&nbsp;</span>

            <?php if(SMARTHOST || ENABLE_IMAP_AUTH == 1) {
                if(isAuditorUser() == 1) { ?>
                   <a href="#" class="btn btn-link" data-bs-toggle="modal" data-bs-target="#bulkRestoreModal" title="<?php print $text_bulk_restore_selected_emails; ?>"><i class="bi bi-send"></i></a>
                <?php } else { ?>
                   <a href="#" class="btn btn-link" onclick="Piler.bulk_restore_messages('<?php print $text_restored; ?>', '');" title="<?php print $text_bulk_restore_selected_emails; ?>"><i class="bi bi-send"></i></a>
                <?php } ?>
            <?php } ?>

                <a href="#" class="btn btn-link" onclick="Piler.download_messages();" title="<?php print $text_bulk_download; ?>"><i class="bi bi-download"></i></a>

            <?php if(ENABLE_DELETE == 1 && isAuditorUser() == 1) { ?>
                <a href="#" class="btn btn-link" data-bs-toggle="modal" data-bs-target="#deleteModal" title="<?php print $text_remove; ?>"><i class="bi bi-trash text-danger"></i></a>
            <?php } ?>

                <input type="text" id="tag_value" name="tag_value" class="tagtext" />
                <a href="#" class="btn btn-link" onclick="Piler.tag_search_results('<?php print $text_tagged; ?>');" title="<?php print $text_tag_selected_messages; ?>"><i class="bi bi-tags tag" title="Tag"></i></a>

          </div>
        </div>


</div>
