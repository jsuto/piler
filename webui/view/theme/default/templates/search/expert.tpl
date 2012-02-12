
<div id="search">
   <div id="search1">

      <div id="ss1" style="border: 0px solid red;">

         <input type="hidden" name="searchtype" id="searchtype" value="expert" />

         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />

      </div>


      <div id="ss1" style="border: 0px solid blue;">
         <div class="row">
            <div class="cell1s"><?php print $text_search_terms; ?>: </div>
            <div class="cell2"><input type="text" id="_search" name="_search" class="advtext" style="width: 100%;" value="<?php if(isset($_search)) { print $_search; } ?>" /></div>
         </div>
      </div>

      <div class="row">
         <div class="cell1s">&nbsp;</div>
         <div class="cell2">
            <button id="button_search" class="active" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0);"><?php print $text_search; ?></button>
            <input type="button" class="advsecondary" onclick="javascript:var a=document.getElementById('_search'); a.value=''; a = document.getElementById('ref'); a.value=''; return false;" value="<?php print $text_cancel; ?>" />
            <input type="button" class="advsecondary" value="<?php print $text_save; ?>" onclick="javascript:send_ajax_post_request('<?php print SAVE_SEARCH_URL; ?>', assemble_search_term(0) + '&save=1', 'A1');" />
            <input type="button" class="load" name="load" onclick="javascript:load_ajax_url('<?php print SAVE_SEARCH_URL; ?>&<?php if(isset($search_args)) { print $search_args; } ?>');" value="<?php print $text_load; ?>..." />
         </div>
      </div>


         <input type="hidden" name="tag" value="" />


   </div>

   <div id="search2">
      <div id="A1"></div>
   </div>

</div>


<div id="AS">
   <div id="resultscontainer" class="empty"></div>
</div>


<?php if(isset($searchterm)) { ?>

   <script>
      fix_search_order('<?php print $sort; ?>', '<?php print $order; ?>');
      load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0);
      load_ajax_url('<?php print SAVE_SEARCH_URL; ?>');
   </script>

<?php } ?>

