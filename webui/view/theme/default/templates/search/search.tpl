
   <div id="search">

      <div id="search1">

            <input type="hidden" name="searchtype" id="searchtype" value="simple" />

            <input type="hidden" name="sort" id="sort" value="date" />
            <input type="hidden" name="order" id="order" value="0" />

        <div id="ss1">
         <div class="row">
            <div class="cell1"><?php print $text_from; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="from" id="from" value="<?php if(isset($from)) { print $from; } ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_to; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="to" id="to" value="<?php if(isset($to)) { print $to; } ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_text; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="subject" id="subject" value="<?php if(isset($subject)) { print $subject; } ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_date; ?>: </div>
            <div class="cell2"><input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" /> <input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1">&nbsp;</div>
            <div class="cell2">
               <button id="button_search" class="active" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0);"><?php print $text_search; ?></button>
               <input type="button" class="advsecondary" onclick="javascript:reset_simple_form(); return false;" value="<?php print $text_cancel; ?>" />
               <input type="button" class="advsecondary" value="<?= $text_save; ?>" onclick="javascript:send_ajax_post_request('<?php print SAVE_SEARCH_URL; ?>', assemble_search_term(count) + '&save=1', 'A1');" />
               <input type="button" class="load" name="load" onclick="javascript:load_ajax_url('<?php print SAVE_SEARCH_URL; ?>&<?php if(isset($search_args)) { print $search_args; } ?>');" value="<?php print $text_load; ?>..." />
            </div>
         </div>

         <input type="hidden" name="tag" value="" />
       </div>

      </div>

      <div id="search2">
         <div id="A1"></div>
      </div>

   </div>


<div id="AS">
   <div id="resultscontainer" class="empty">
   </div>
</div>


<?php if(isset($searchterm)) { ?>

   <script>
      fix_search_order('<?php print $sort; ?>', '<?php print $order; ?>');
      load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0);
      load_ajax_url('<?php print SAVE_SEARCH_URL; ?>');
   </script>

<?php } ?>

