
<?php if(count($searchterms) > 0) { ?>
               <form name="savesearchform" action="advanced.php" method="post" style="margin-bottom: 5px;">
          
               <select name="searchterm" id="searchterm" class="select1" multiple size="<?php print $select_size; ?>" onChange="javascript:fixup_save_post_action_url(); this.form.submit(); return false;">
                  <option value="search.php"<?php if("" == $_SERVER['QUERY_STRING']) { ?> selected="selected"<?php } ?>><?php print $text_empty_search_criteria; ?></option>

               <?php foreach($searchterms as $term) {
                  $term_value = assemble_search_url($term['term']);
               ?>
                  <option value="<?php print $term['term']; ?>&loaded=1"><?php print $term_value; ?></option>
               <?php } ?>
               </select>

            </form> 

            <input type="button" class="load" onclick="javascript:document.getElementById('A1').innerHTML = '<p>&nbsp;</p>'; " value="<?php print $text_cancel; ?>" />

<?php } ?>

