<div id="messagelistcontainer" class="boxlistcontent" style="top:0">

  <div id="results">

<?php if(count($terms) > 0){ ?>
        <table class="table table-striped">
            <thead><tr><th>Query Name</th><th>&nbsp;</th></thead>
<?php foreach($terms as $term) {
         parse_str($term['term'], $s);
         if(isset($s['search'])) {
?>
         <tr>
            <td><a href="#" onclick="Piler.load_search_results_for_saved_query('<?php print urlencode($term['term']); ?>');"><?php print htmlspecialchars($s['search']); ?></a> | <a href="#" onclick="Piler.remove_saved_search_term(<?php print $term['ts']; ?>); return false;"><i class="bi bi-trash text-danger"></i></a></td>
            <td>&nbsp;</td>
         </tr>
<?php } } ?>

        </table>

<?php } else if(count($terms) == 0) { ?>
            <div class="alert alert-error">No saved queries yet</div>
<?php } ?>

  </div>
</div>
