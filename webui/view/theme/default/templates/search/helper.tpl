
        <div id="messagelistcontainer" class="boxlistcontent" style="top:0">

   <div id="results">

         <div class="resultrow">
<?php if($n > 0){ ?>
            <div class="cell1r" style="vertical-align:middle;"><input type="checkbox" id="bulkcheck" name="bulkcheck" value="1" checked="checked" class="restorebox" onchange="javascript:toggle_bulk_check(); return false;" /></div>
            <div class="cell1q" style="text-align: center;"><a href="#" onclick="javascript: download_selected_emails('<?php print BULK_RESTORE_URL; ?>'); return false;"><img style="margin: 0px 0 -2px 0; border: 0px solid black;" src="<?php print ICON_DOWNLOAD; ?>" width="18" height="18" alt="aaa" border="0"></a></div>
            <div class="cell3 date">
               <?php print $text_date; ?>
               <a href="#" onclick="script:fix_search_order('date', 1); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('date', 0); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3 title">
               <?php print $text_from; ?>
               <a href="#" onclick="script:fix_search_order('from', 1); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('from', 0); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3 title">
               <?php print $text_subject; ?>
               <a href="#" onclick="script:fix_search_order('subj', 1); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('subj', 0); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3 title">
               <?php print $text_size; ?>
               <a href="#" onclick="script:fix_search_order('size', 1); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('size', 0); <?php if(isset($_POST['ref'])) { ?>add_message_reference_to_form('<?php print $_POST['ref']; ?>');<?php } ?> load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3">&nbsp;</div>
            <div class="cell3">&nbsp;</div>
            <div class="cell3">&nbsp;</div>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
            <div class="cell3">&nbsp;</div>
         <?php } ?>
<?php } else if($n == 0) { ?>
            <div class="cell3 error"><?php print $text_empty_search_result; ?></div>
<?php } ?>
         </div>



<?php $i=0; foreach ($messages as $message) { $i++; ?>

         <div id="e_<?php print $message['id']; ?>" class="resultrow<?php if($i % 2) { ?> odd<?php } ?><?php if($message['spam'] == 1) { ?> spam<?php } ?>">
            <div class="cell5 restore"><input type="checkbox" id="r_<?php print $message['id']; ?>" name="r_<?php print $message['id']; ?>" value="iiii" checked="checked" class="restorebox" /></div>
            <div class="cell5 id"><a href="#" onclick="script:load_url_with_get('<?php print SITE_URL; ?>message.php/<?php print $message['id']; ?>', 'mailpreviewframe'); return false;"><?php print ($page*$page_len) + $i; ?>.</a></div>
            <div class="cell5 date"><?php print $message['date']; ?></div>
            <div class="cell5 from"><?php if($message['from'] != $message['shortfrom']) { ?><a href="#" title="<?php print preg_replace("/&/", "&amp;", $message['from']); ?>"><?php print $message['shortfrom']; ?></a><?php } else { print $message['from']; } ?></div>
            <div class="cell5 subject"><a href="#" <?php if($message['subject'] != $message['shortsubject']) { ?>title="<?php print preg_replace("/&/", "&amp;", $message['subject']); ?>"<?php } ?> onclick="script:load_url_with_get('<?php print SITE_URL; ?>message.php/<?php print $message['id']; ?>', 'mailpreviewframe'); return false;"><?php if($message['subject'] != $message['shortsubject']) { print $message['shortsubject']; } else { print $message['subject']; } ?></a><?php if($message['reference']) { ?> <a href="#" title="<?php print $text_conversation_available; ?>" onclick="script:add_message_reference_to_form('<?php print $message['reference']; ?>'); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); a = document.getElementById('ref'); a.value=''; return false;">[+]</span></a><?php } ?></div>

            <div class="cell5 size"><?php print $message['size']; ?></div>
            <div class="cell5"><?php if($message['attachments'] > 0) { ?><img src="<?php print ICON_ATTACHMENT; ?>" alt="" width="16" height="18" /><?php } else { ?>&nbsp;<?php } ?></div>
            <div class="cell5"><?php if($message['note']) { ?><a href="#" title="<?php print preg_replace("/\"/", "&quot;", $message['note']); ?>"><img src="<?php print ICON_NOTES; ?>" alt="akakaka" width="16" height="16" /></a><?php } else { ?>&nbsp;<?php } ?></div>
            <div class="cell5"><?php if($message['tag']) { ?><a href="#" title="<?php print preg_replace("/\"/", "&quot;", $message['tag']); ?>"><img src="<?php print ICON_TAG; ?>" alt="" /></a><?php } else { ?>&nbsp;<?php } ?></div>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
            <div class="cell5 verification"><img src="<?php if($message['verification'] == 1){ print ICON_GREEN_OK; } else { print ICON_RED_X; } ?>" alt="verification status" /></div>
         <?php } ?>
         </div>

<?php } ?>



   </div>

        </div>
        <div class="boxfooter" style="border: 1px solid black;">


<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), <?php print $prev_page; ?>); return false;"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), <?php print $next_page; ?>); return false;">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), <?php print $total_pages; ?>); return false;"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>

<?php } else { ?>&nbsp;<?php } ?>

         <input type="hidden" id="tag_keys" name="tag_keys" value="<?php print $all_ids; ?>" />
         <input type="hidden" id="_ref" name="_ref" value="<?php if(isset($_ref)) { print $_ref; } ?>" />

      <input type="button" class="restore" value="<?php print $text_bulk_restore_selected_emails; ?>" onclick="javascript: restore_selected_emails('<?php print BULK_RESTORE_URL; ?>'); show_message('messagebox1', '<p><?php print $text_restored; ?></p>', 0.8); " />

      <?php print $text_tag_search_results; ?>: <input type="text" id="tag_value" name="tag_value" class="tagtext" /> <input type="button" class="tag" onclick="javascript: tag_search_results('<?php print SEARCH_TAG_URL; ?>'); show_message('messagebox1', '<p><?php print $text_tagged; ?></p>', 0.8); var __ref = document.getElementById('_ref').value; if(__ref) { add_message_reference_to_form(__ref); } load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0); return false;" value="OK" />


   </div>

</div>

