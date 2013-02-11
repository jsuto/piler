
        <div id="messagelistcontainer" class="boxlistcontent">

   <div id="results">

<?php if($n > 0) { ?>

<?php $i=0; foreach ($messages as $message) { ?>

         <div id="e_<?php print $message['id']; ?>" class="resultrow new<?php if($i % 2) { ?> odd<?php } ?><?php if($message['spam'] == 1) { ?> spam<?php } ?>">
            <div class="resultcell restore"><input type="checkbox" id="r_<?php print $message['id']; ?>" name="r_<?php print $message['id']; ?>" value="iiii" <?php if(SEARCH_RESULT_CHECKBOX_CHECKED == 1) { ?>checked="checked"<?php } ?> class="restorebox" /></div>
            <div class="resultcell id"><a href="#" onclick="Piler.view_message(<?php print $i; ?>);"><?php print ($page*$page_len) + $i + 1; ?>.</a></div>
            <div class="resultcell date"><?php print $message['date']; ?></div>
            <div class="resultcell from"><?php if($message['from'] != $message['shortfrom']) { ?><span title="<?php print $message['from']; ?>"><?php print $message['shortfrom']; ?></span><?php } else { print $message['from']; } ?></div>
            <div class="resultcell to"><?php if($message['to'] != $message['shortto']) { ?><span title="<?php print $message['to']; ?>"><?php print $message['shortto']; ?>*</span><?php } else { print $message['to']; } ?></div>
            <div class="resultcell subject"><a href="#" <?php if($message['subject'] != $message['shortsubject']) { ?>title="<?php print $message['subject']; ?>"<?php } ?> onclick="Piler.view_message(<?php print $i; ?>);"><?php if($message['subject'] != $message['shortsubject']) { print $message['shortsubject']; } else { print $message['subject']; } ?></a><?php if($message['reference']) { ?> <a href="#" title="<?php print $text_conversation_available; ?>" onclick="$('#ref').val('<?php print $message['reference']; ?>'); Piler.expert(this);">[+]</span></a><?php } ?></div>

            <div class="resultcell size"><?php print $message['size']; ?></div>
            <div class="resultcell end"><?php if($message['attachments'] > 0) { ?><img src="<?php print ICON_ATTACHMENT; ?>" alt="" width="16" height="18" /><?php } else { ?>&nbsp;<?php } ?></div>
            <div class="resultcell end"><?php if($message['note']) { ?><a href="#" title="<?php print $message['note']; ?>"><img src="<?php print ICON_NOTES; ?>" alt="akakaka" width="16" height="16" /></a><?php } else { ?>&nbsp;<?php } ?></div>
            <div class="resultcell end"><?php if($message['tag']) { ?><a href="#" title="<?php print $message['tag']; ?>"><img src="<?php print ICON_TAG; ?>" alt="" /></a><?php } else { ?>&nbsp;<?php } ?></div>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
            <div class="resultcell verification"><img src="<?php if($message['verification'] == 1){ print ICON_GREEN_OK; } else { print ICON_RED_X; } ?>" alt="verification status" /></div>
         <?php } ?>
         </div>

<?php $i++; } ?>

<?php } else if($n == 0) { ?>
            <div class="cell3 error"><?php print $text_empty_search_result; ?></div>
<?php } ?>


   </div>

        </div>
        <div class="boxfooter">


<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(0);">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $prev_page; ?>);"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="Piler.navigation(<?php print $next_page; ?>);">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="Piler.navigation(<?php print $total_pages; ?>);"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>

<?php } else { ?>&nbsp;<?php } ?>

         <input type="hidden" id="tag_keys" name="tag_keys" value="<?php print $all_ids; ?>" />
         <input type="hidden" id="_ref" name="_ref" value="<?php if(isset($_ref)) { print $_ref; } ?>" />

      <input type="button" class="restore" value="<?php print $text_bulk_restore_selected_emails; ?>" onclick="Piler.bulk_restore_messages('<?php print $text_restored; ?>'); " />

      <?php print $text_tag_search_results; ?>: <input type="text" id="tag_value" name="tag_value" class="tagtext" /> <input type="button" class="tag" onclick="Piler.tag_search_results('<?php print $text_tagged; ?>');" value="OK" />


   </div>

</div>

