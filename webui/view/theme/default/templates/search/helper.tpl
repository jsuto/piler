<div id="sspinner" class="alert alert-info lead"><i class="icon-spinner icon-spin icon-2x pull-left"></i><?php print $text_working; ?></div>
<div id="messagelistcontainer" class="boxlistcontent">
    <?php if($n > 0) { ?>
    <table id="results" class="table table-striped table-condensed">
      <thead>
        <tr>
          <th id="restore-header"><input type="checkbox" id="bulkcheck" name="bulkcheck" value="1" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" onclick="Piler.toggle_bulk_check();" /></th>
          <th id="id-header">&nbsp;</th>
          <th id="date-header">
             <?php print $text_date; ?>
             <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="from-header">
             <?php print $text_from; ?>
             <a xid="from" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="from" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="to-header">
             <?php print $text_to; ?>
             <a xid="to" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="to" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="subject-header">
             <?php print $text_subject; ?>
             <a xid="subj" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="subj" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="size-header">
             <?php print $text_size; ?>
             <a xid="size" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="size" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="spam-header"><i class="spam icon-warning-sign icon-large" title="<?php print $text_spam_flag; ?>"></i></th>
          <th id="attachment-header"><i class="attachment icon-paper-clip icon-large" title="<?php print $text_attachment_flag; ?>"></i></th>
          <th id="note-header"><i class="note icon-file-alt icon-large" title="<?php print $text_notes_flag; ?>"></i></th>
          <th id="tag-header"><i class="tag icon-tags icon-large" title="<?php print $text_tag_flag; ?>"></i>
     <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
          <th id="verify-header"><i class="verified icon-ok-sign icon-large" title="<?php print $text_verified_flag; ?>"></i></th>
     <?php } ?>
       </tr>
      </thead>
      <tbody>
    <?php $i=0; foreach ($messages as $message) { ?>
            
         <tr onmouseover="Piler.current_message_id = <?php print $message['id']; ?>; return false;" id="e_<?php print $message['id']; ?>" class="resultrow new" onclick="Piler.view_message_by_pos(<?php print $i; ?>);">
            <td id="c1_r<?php print $i; ?>" class="resultcell restore"><input type="checkbox" id="r_<?php print $message['id']; ?>" name="r_<?php print $message['id']; ?>" value="iiii" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" /></td>
            <td id="c2_r<?php print $i; ?>" class="resultcell id"><?php print ($page*$page_len) + $i + 1; ?></td>
            <td id="c3_r<?php print $i; ?>" class="resultcell date"><?php print $message['date']; ?></td>
            <td id="c4_r<?php print $i; ?>" class="resultcell from"><?php if($message['from'] != $message['shortfrom']) { ?><span title="<?php print $message['from']; ?>"><?php print $message['shortfrom']; ?></span><?php } else { print $message['from']; } ?></td>
            <td id="c5_r<?php print $i; ?>" class="resultcell to"><?php if($message['to'] != $message['shortto']) { ?><span title="<?php print $message['to']; ?>"><?php print $message['shortto']; ?>&nbsp;<i class=" muted icon-group"></i></span><?php } else { print $message['to']; } ?></td>
            <td id="c6_r<?php print $i; ?>" class="resultcell subject"><a href="#" <?php if($message['subject'] != $message['shortsubject']) { ?>title="<?php $message['subject']; ?>"<?php } ?> onclick="Piler.view_message_by_pos(<?php print $i; ?>);"><?php if($message['subject'] != $message['shortsubject']) { print $message['shortsubject']; } else { print $message['subject']; } ?></a><?php if($message['reference']) { ?> <a href="#" title="<?php print $text_conversation_available; ?>" onclick="$('#ref').val('<?php print $message['reference']; ?>'); Piler.expert(this);">[+]</span></a><?php } ?></td>
            <td id="c7_r<?php print $i; ?>" class="resultcell size"><?php print $message['size']; ?></td>
            <td id="c8_r<?php print $i; ?>" class="resultcell end"><?php if($message['spam'] == 1) { ?><i class="spam icon-warning-sign icon-large" title="<?php print $text_spam_flag; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
            <td id="c9_r<?php print $i; ?>" class="resultcell end"><?php if($message['attachments'] > 0) { ?><i class="attachment icon-paper-clip icon-large" title="<?php print $text_attachment_flag; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
            <td id="c10_r<?php print $i; ?>" class="resultcell end"><?php if($message['note']) { ?><i class="note icon-file-alt icon-large" title="<?php print $text_notes_flag; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
            <td id="c11_r<?php print $i; ?>" class="resultcell end"><?php if($message['tag']) { ?><i class="tag icon-tags icon-large" title="<?php print $text_tag_flag; ?>"></i><?php } else { ?>&nbsp;<?php } ?></td>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { if ($message['verification'] == 1) {?>
            <td id="c12_r<?php print $i; ?>" class="resultcell end"><i class="verified icon-ok-sign icon-large" title="<?php print $text_verified_flag; ?>"></i></td>
         <?php } else { ?>
            <td id="c12_r<?php print $i; ?>" class="resultcell end"><i class="unverified icon-remove-sign icon-large" title="<?php print $text_unverified_flag; ?>"></i></td>
         <?php } } ?>
         </tr>

    <?php $i++; } ?>
      </tbody>
      
      <tfoot>
        <tr>
          <th id="restore-header"><input type="checkbox" id="bulkcheck" name="bulkcheck" value="1" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" onclick="Piler.toggle_bulk_check();" /></th>
          <th id="id-header">&nbsp;</th>
          <th id="date-header">
             <?php print $text_date; ?>
             <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="from-header">
             <?php print $text_from; ?>
             <a xid="from" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="from" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="to-header">
             <?php print $text_to; ?>
             <a xid="to" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="to" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="subject-header">
             <?php print $text_subject; ?>
             <a xid="subj" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="subj" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="size-header">
             <?php print $text_size; ?>
             <a xid="size" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
             <a xid="size" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
          </th>
          <th id="spam-header"><i class="spam icon-warning-sign icon-large" title="<?php print $text_spam_flag; ?>"></i></th>
          <th id="attachment-header"><i class="attachment icon-paper-clip icon-large" title="<?php print $text_attachment_flag; ?>"></i></th>
          <th id="note-header"><i class="note icon-file-alt icon-large" title="<?php print $text_notes_flag; ?>"></i></th>
          <th id="tag-header"><i class="tag icon-tags icon-large" title="<?php print $text_tag_flag; ?>"></i>
     <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
          <th id="verify-header"><i class="verified icon-ok-sign icon-large" title="<?php print $text_verified_flag; ?>"></i></th>
     <?php } ?>
       </tr>
      </tfoot>
      
    </table>

    <?php } else if($n == 0) { ?>
                <div class="alert alert-block alert-error lead"><i class="icon-exclamation-sign icon-2x pull-left"></i> <?php print $text_empty_search_result; ?></div>
    <?php } ?>

</div>

<div id="messagelistfooter" class="boxfooter">
       <div id="pagingrow" class="pull-left">
    <?php if($n > 0){ ?>
            &nbsp;
            <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(0);"><i class="icon-double-angle-left icon-large"></i></a><?php } else { ?><span class="navlink"><i class="icon-double-angle-left icon-large muted"></i></span><?php } ?>
            &nbsp;
            <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $prev_page; ?>);"><i class="icon-angle-left icon-large"></i></a><?php } else { ?><span class="navlink"><i class="icon-angle-left icon-large muted"></i></span><?php } ?>
            &nbsp;
            <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
            &nbsp;
            <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $next_page; ?>);"><i class="icon-angle-right icon-large"></i></a> <?php } else { ?><span class="navlink"><i class="icon-angle-right icon-large muted"></i></span><?php } ?>
            &nbsp;
            <?php if($page < $total_pages) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $total_pages; ?>);"><i class="icon-double-angle-right icon-large"></i></a><?php } else { ?> <span class="navlink"><i class="icon-double-angle-right icon-large muted"></i></span><?php } ?>
            &nbsp;
    <?php } else { print $text_none_found; } ?>
        </div>
        <div id="functionrow" class="pull-right">
            <input type="hidden" id="tag_keys" name="tag_keys" value="<?php print $all_ids; ?>" />
            <input type="hidden" id="_ref" name="_ref" value="<?php if(isset($_ref)) { print $_ref; } ?>" />
            <span id="tagbox" class="input-prepend input-append">
                <span class="add-on"><i class="icon-tags icon-large" title="Tag"></i>&nbsp;<?php print $text_tag_search_results; ?>:</span>
                <input type="text" id="tag_value" name="tag_value" class="tagtext" />
                <input type="button" class="btn btn-info" onclick="Piler.tag_search_results('<?php print $text_tagged; ?>');" value="Tag" />
            </span>
            <input type="button" class="btn btn-custom btn-inverse" value="<?php print $text_bulk_restore_selected_emails; ?>" onclick="Piler.bulk_restore_messages('<?php print $text_restored; ?>'); " />&nbsp;
            <input type="button" class="btn btn-custom btn-inverse" value="<?php print $text_bulk_download; ?>" onclick="Piler.download_messages();" />
       </div>
</div>
