
   <div id="resultscontainer" class="empty" style="width: 990px; border:0px solid green; "></div>

<?php if(isset($searchterm)) { ?>

   <script>
      fix_search_order('<?php print $sort; ?>', '<?php print $order; ?>');
      load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count, ''), 0);
      load_ajax_url('<?php print SAVE_SEARCH_URL; ?>');
   </script>

<?php } ?>

