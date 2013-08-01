
        <div id="messagelistcontainer" class="boxlistcontent">

   <div id="results">

<?php if($n > 0) { ?>
       <table class="table table-condensed" style="border: 0px solid blue; width: 98%;">
          <thead>

    <tr>
      <td><input type="checkbox" id="bulkcheck" name="bulkcheck" value="1" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> onclick="Piler.toggle_bulk_check('');" /></td>
      <td>
      <?php if(Registry::get('auditor_user') == 1 || BULK_DOWNLOAD_FOR_USERS == 1) { ?>
          <a href="#"><img class="download_icon" src="<?php print ICON_DOWNLOAD; ?>" width="18" height="18" alt="aaa" border="0" onclick="Piler.download_messages();" /></a></td>
      <?php } ?>
      <td>
         <strong><?php print $text_date; ?></strong>
         <a xid="date" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
         <a xid="date" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
      </td>
      <td>
         <strong><?php print $text_from; ?></strong>
         <a xid="from" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
         <a xid="from" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
      </td>
      <td>
         <strong><?php print $text_to; ?></strong>
      </td>
      <td>
         <strong><?php print $text_subject; ?><strong>
         <a xid="subj" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
         <a xid="subj" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
      </td>
      <td>
         <strong><?php print $text_size; ?></strong>
         <a xid="size" xorder="1" onclick="Piler.changeOrder(this);"><i class="icon-chevron-up"></i></a>
         <a xid="size" xorder="0" onclick="Piler.changeOrder(this);"><i class="icon-chevron-down"></i></a>
      </td>
      <td><i class="icon-file"></i></div>
      <td><i class="icon-bookmark"></i></div>
      <td><i class="icon-tag"></i></div>
   <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
      <td>&nbsp;</td>
   <?php } ?>
   </tr>

        </thead>

        <tbody>
<?php $i=0; foreach ($messages as $message) { ?>
         <tr onmouseover="Piler.current_message_id = <?php print $message['id']; ?>; return false;" id="e_<?php print $message['id']; ?>" class="resultrow new<?php if($i % 2) { ?> odd<?php } ?><?php if($message['spam'] == 1) { ?> spam<?php } ?><?php if($message['yousent'] == 1) { ?> sent<?php } ?>">
            <td><input type="checkbox" id="r_<?php print $message['id']; ?>" name="r_<?php print $message['id']; ?>" value="iiii" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> /></td>
            <td><a href="#" onclick="Piler.view_message_by_pos(<?php print $i; ?>);"><?php print ($page*$page_len) + $i + 1; ?>.</a></td>
            <td><?php print $message['date']; ?></td>
            <td><?php if($message['from'] != $message['shortfrom']) { ?><span title="<?php print $message['from']; ?>"><?php print $message['shortfrom']; ?></span><?php } else { print $message['from']; } ?></td>
            <td><?php if($message['to'] != $message['shortto']) { ?><span title="<?php print $message['to']; ?>"><?php print $message['shortto']; ?><i class="icon-user"></i></span><?php } else { print $message['to']; } ?></td>
            <td><a href="#" <?php if($message['spam'] == 1) { ?>class="spam"<?php } ?> <?php if($message['subject'] != $message['shortsubject']) { ?>title="<?php print $message['subject']; ?>"<?php } ?> onclick="Piler.view_message_by_pos(<?php print $i; ?>);"><?php if($message['subject'] != $message['shortsubject']) { print $message['shortsubject']; } else { print $message['subject']; } ?></a><?php if($message['reference']) { ?> <a href="#" title="<?php print $text_conversation_available; ?>" onclick="$('#ref').val('<?php print $message['reference']; ?>'); Piler.expert(this);">[+]</span></a><?php } ?></td>

            <td><?php print $message['size']; ?></td>
            <td><?php if($message['attachments'] > 0) { ?><i class="icon-file"></i><?php } ?></td>
            <td><?php if($message['note']) { ?><i class="icon-bookmark" title="<?php print $message['note']; ?>"></i><?php } ?></td>
            <td><?php if($message['tag']) { ?><i class="icon-tag" title="<?php print $message['tag']; ?>"></i><?php } ?></td>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
            <td><i class="icon-<?php if($message['verification'] == 1){ ?>ok<?php } else { ?>remove<?php } ?>-sign" title="verification status"></i></td>
         <?php } ?>
         </tr>

<?php $i++; } ?>
       </tbody>
      </table>



<?php } else if($n == 0) { ?>
    <div class="alert">
       <strong><?php print $text_empty_search_result; ?></strong>
    </div>

<?php } ?>


   </div>

        </div>


        <div class="boxfooter">
           <form class="form-inline sleek" name="tagging">

<?php if($n >= $page_len){ ?>
   <span class="piler-right-margin">
         <?php if($page > 0) { ?><a href="#" onclick="Piler.navigation(0);">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" onclick="Piler.navigation(<?php print $prev_page; ?>);"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>

         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>

         <?php if($next_page <= $total_pages){ ?><a href="#" onclick="Piler.navigation(<?php print $next_page; ?>);">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" onclick="Piler.navigation(<?php print $total_pages; ?>);"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
   </span>

<?php } else { ?>&nbsp;<?php } ?>

              <button class="btn piler-right-margin" onclick="Piler.bulk_restore_messages('<?php print $text_restored; ?>'); return false;"><?php print $text_bulk_restore_selected_emails; ?></button>

              <input type="text" id="tag_value" name="tag_value" class="input-xlarge" placeholder="<?php print $text_tag_selected_messages; ?>" />
              <button class="btn" onclick="Piler.tag_search_results('<?php print $text_tagged; ?>'); return false;" >OK</button>
           </form>

        </div>

