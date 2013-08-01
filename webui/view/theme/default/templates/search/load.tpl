<div id="messagelistcontainer" class="boxlistcontent" style="top:0">

   <div id="results">
         
<?php if(count($terms) > 0){ ?>
        <table class="table table-striped">
            <thead><tr><th>Query Name</th><th>D?</th></thead>
<?php foreach($terms as $term) {
         parse_str($term['term'], $s);
         if(isset($s['search'])) {
?>
         <tr>
            <td><a href="#" onclick="Piler.load_search_results_for_saved_query('<?php print urlencode($term['term']); ?>');"><?php print $s['search']; ?></a> | <a href="#" class="menulink" onclick="Piler.remove_saved_search_term(<?php print $term['ts']; ?>); return false;"><?php print $text_remove; ?></a></td>
            <td>&nbsp;</td>
         </tr>
<?php } } ?>

        </table>

<?php } else if(count($terms) == 0) { ?>
            <div class="alert alert-error"><?php print $text_empty_search_result; ?></div>
<?php } ?>

       </div>
   </div>


<?php

?>
