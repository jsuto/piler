
        <div id="messagelistcontainer" class="boxlistcontent" style="top:0">

   <div id="results">

         <div class="resultrow">
<?php if(count($terms) > 0){ ?>

<?php } else if(count($terms) == 0) { ?>
            <div class="cell3 error"><?php print $text_empty_search_result; ?></div>
<?php } ?>
         </div>

<?php foreach($terms as $term) {
         parse_str($term['term'], $s);
         if(isset($s['search'])) {
?>
         <div class="resultrow">
            <a href="#" onclick="javascript:load_search_results('<?php print SEARCH_HELPER_URL; ?>', '<?php print $term['term']; ?>', 0); var a=document.getElementById('_search'); var v=decodeURIComponent('<?php print urlencode($s['search']); ?>'); v = v.replace(/\+/g, ' '); a.value=v; "><?php print $s['search']; ?></a></br />
         </div>
<?php } } ?>

       </div>
   </div>


<?php

?>
