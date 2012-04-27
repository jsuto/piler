
<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), <?php print $prev_page; ?>); return false;"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), <?php print $next_page; ?>); return false;">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), <?php print $total_pages; ?>); return false;"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>
<?php } if($n > 0){ ?>
      <div id="tagbox"<?php if($n < $page_len) { ?> style="margin-left: 265px;"<?php } ?>>
         <input type="hidden" id="tag_keys" name="tag_keys" value="<?php print $all_ids; ?>" />
         <input type="hidden" id="_ref" name="_ref" value="<?php if(isset($_ref)) { print $_ref; } ?>" />

         <?php print $text_tag_search_results; ?>: <input type="text" id="tag_value" name="tag_value" class="tagtext" /> <input type="button" class="tag" onclick="javascript: tag_search_results('<?php print SEARCH_TAG_URL; ?>'); var __ref = document.getElementById('_ref').value; if(__ref) { add_message_reference_to_form(__ref); } load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;" value="OK" />
      </div>

<?php } else { ?>&nbsp;<?php } ?>
   </div>

</div>


<div id="resultscontainer"<?php if($n <= 0) { ?> class="empty"<?php } ?>>

   <div id="results">


         <div class="resultrow">
<?php if($n > 0){ ?>
            <div class="cell1">&nbsp;</div>
            <div class="cell3 date">
               <?php print $text_date; ?>
               <a href="#" onclick="script:fix_search_order('date', 1); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('date', 0); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3 title">
               <?php print $text_from; ?>
               <a href="#" onclick="script:fix_search_order('from', 1); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('from', 0); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3 title">
               <?php print $text_subject; ?>
               <a href="#" onclick="script:fix_search_order('subj', 1); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('subj', 0); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cell3 title">
               <?php print $text_size; ?>
               <a href="#" onclick="script:fix_search_order('size', 1); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('size', 0); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
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
            <div class="cell5 id"><?php print ($page*$page_len) + $i; ?>.</div>
            <div class="cell5 date"><?php print $message['date']; ?></div>
            <div class="cell5 from"><?php if($message['from'] != $message['shortfrom']) { ?><span onmouseover="Tip('<?php print preg_replace("/&/", "&amp;", $message['from']); ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip();"><?php print $message['shortfrom']; ?></span><?php } else { print $message['from']; } ?></div>
            <div class="cell5 subject"><a class="messagelink<?php if($message['spam'] == 1) { ?> spam<?php } ?>" href="<?php print SITE_URL; ?>message.php/<?php print $message['piler_id']; ?>"><?php if($message['subject'] != $message['shortsubject']) { ?><span onmouseover="Tip('<?php print preg_replace("/&/", "&amp;", $message['subject']); ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip();"><?php print $message['shortsubject']; ?></span><?php } else { print $message['subject']; } ?></a><?php if($message['reference']) { ?> <a href="#" onclick="script:add_message_reference_to_form('<?php print $message['reference']; ?>'); load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); a = document.getElementById('ref'); a.value=''; return false;"><span onmouseover="Tip('<?php print $text_conversation_available; ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip();">[+]</span></a><?php } ?></div>
            <div class="cell5 size"><?php print $message['size']; ?></div>
            <div class="cell5"><?php if($message['attachments'] > 0) { ?><img src="<?php print ICON_ATTACHMENT; ?>" alt="" width="16" height="18" /><?php } else { ?>&nbsp;<?php } ?></div>
            <div class="cell5"><?php if($message['tag']) { ?><span onmouseover="Tip('<?php print $message['tag']; ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip();"><img src="<?php print ICON_TAG; ?>" alt="" /></span><?php } else { ?>&nbsp;<?php } ?></div>
         <?php if(ENABLE_ON_THE_FLY_VERIFICATION == 1) { ?>
            <div class="cell5 verification"><img src="<?php if($message['verification'] == 1){ print ICON_GREEN_OK; } else { print ICON_RED_X; } ?>" alt="verification status" /></div>
         <?php } ?>
         </div>

<?php } ?>



<?php if($n > 0) { for($i=$i; $i<$page_len;$i++){ ?>
      <div class="resultrow"></div>
<?php if($i == 20) { break; } ?>
<?php } } ?>

   </div>

</div>


<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0); return false;">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), <?php print $prev_page; ?>); return false;"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), <?php print $next_page; ?>); return false;">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), <?php print $total_pages; ?>); return false;"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>

<?php } else { ?>&nbsp;<?php } ?>
   </div>
</div>



