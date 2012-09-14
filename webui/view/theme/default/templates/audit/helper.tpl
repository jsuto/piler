
<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), <?php print $prev_page; ?>); return false;"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), <?php print $next_page; ?>); return false;">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), <?php print $total_pages; ?>); return false;"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>

<?php } else { ?>&nbsp;<?php } ?>
   </div>
</div>




<div id="auditresultscontainer"<?php if($n <= 0) { ?> class="empty"<?php } ?> style="border: 0px solid red;">

   <div id="results">


         <div class="resultrow">
<?php if($n > 0){ ?>
            <div class="cellaudit title">
               <?php print $text_date; ?>
               <a href="#" onclick="script:fix_search_order('date', 1); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('date', 0); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cellaudit title">
               <?php print $text_user; ?>
               <a href="#" onclick="script:fix_search_order('user', 1); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('user', 0); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>

            <div class="cellaudit title">
               <?php print $text_ipaddr; ?>
               <a href="#" onclick="script:fix_search_order('ipaddr', 1); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('ipaddr', 0); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>

            <div class="cellaudit title">
               <?php print $text_action; ?>
               <a href="#" onclick="script:fix_search_order('action', 1); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('action', 0); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>
            <div class="cellaudit title">
               <?php print $text_description; ?>
               <a href="#" onclick="script:fix_search_order('description', 1); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_UP; ?>" alt="" border="0"></a>
               <a href="#" onclick="script:fix_search_order('description', 0); load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;"><img src="<?php print ICON_ARROW_DOWN; ?>" alt="" border="0"></a>
            </div>

            <div class="cellaudit title">
               <?php print $text_ref; ?>
            </div>

<?php } else if($n == 0) { ?>
            <div class="cell3 error"><?php print $text_empty_search_result; ?></div>
<?php } ?>
         </div>



<?php $i=0; foreach ($messages as $message) { $i++; ?>

         <div class="resultrow<?php if($i % 2) { ?> odd<?php } ?>">
            <div class="cellaudit date"><?php print $message['date']; ?></div>
            <div class="cellaudit"><?php print $message['email']; ?></div>
            <div class="cellaudit"><?php print $message['ipaddr']; ?></div>
            <div class="cellaudit"><?php print $actions[$message['action']]; ?></div>
            <div class="cellaudit"><?php if($message['description'] != $message['shortdescription']) { ?><span onmouseover="Tip('<?php print preg_replace("/&/", "&amp;", $message['description']); ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip()"><?php print $message['shortdescription']; ?></span><?php } else { print $message['description']; } ?></div>
            <div class="cellaudit"><?php if($message['piler_id']) { ?><a class="messagelink" href="<?php print SITE_URL; ?>message.php/<?php print $message['piler_id']; ?>"><?php } ?><?php print $message['id']; ?><?php if($message['piler_id']) { ?></a><?php } ?></div>
         </div>

<?php } ?>



<?php if($n > 0) { for($i=$i; $i<$page_len;$i++){ ?>
      <div class="resultrow"></div>
<?php } } ?>

   </div>

</div>


<div id="pagenav">
   <div class="navrow">
<?php if($n >= $page_len){ ?>
      <div id="pagingleft">
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0); return false;">&lt;&lt;</a> &nbsp; <?php } else { ?><span class="navlink">&lt;&lt; &nbsp; </span><?php } ?>
         <?php if($page > 0) { ?><a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), <?php print $prev_page; ?>); return false;"> &lt; </a> <?php } else { ?><span class="navlink"> &lt; </span><?php } ?>
      </div>

      <div id="pagingcenter">
         <?php print $hits_from; ?>-<?php print $hits_to; ?>, <?php print $text_total; ?>: <?php print $n; ?>
      </div>

      <div id="pagingright">
         <?php if($next_page <= $total_pages){ ?><a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), <?php print $next_page; ?>); return false;">&gt; </a> <?php } else { ?><span class="navlink">&gt; </span><?php } ?>
         <?php if($page < $total_pages) { ?> &nbsp; <a href="#" class="navlink" onclick="script:load_audit_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), <?php print $total_pages; ?>); return false;"> &gt;&gt; </a><?php } else { ?> <span class="navlink"> &nbsp; &gt;&gt;</span><?php } ?>
      </div>

<?php } else { ?>&nbsp;<?php } ?>
   </div>
</div>



