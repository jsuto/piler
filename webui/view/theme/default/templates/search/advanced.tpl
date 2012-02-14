
<div id="search">
   <div id="search1">

      <div id="ss1" style="border: 0px solid red;">

         <input type="hidden" name="searchtype" id="searchtype" value="advanced" />

         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />
         <input type="hidden" name="ref" id="ref" value="" />



<div id="myid">

         <div id="myid0" class="row" style="margin:0; padding:0; border: 0px solid blue; float: left;">

                  <div class="cell1s">
                           <select name="f[]" id="key0" class="advselect">
                              <option value="from"<?php if(isset($key0) && $key0 == 0) { ?> selected="selected"<?php } ?>><?php print $text_from; ?></option>
                              <option value="to"<?php if(isset($key0) && $key0 == 1) { ?> selected="selected"<?php } ?>><?php print $text_to; ?></option>
                              <option value="subject"<?php if(isset($key0) && $key0 == 2) { ?> selected="selected"<?php } ?>><?php print $text_subject; ?></option>
                              <option value="body"<?php if(isset($key0) && $key0 == 3) { ?> selected="selected"<?php } ?>><?php print $text_body; ?></option>
                           </select>
                  </div>
                  <div class="cell2">
                           <input type="text" class="advtext" name="v[]" id="val0" value="<?php if(isset($val0)) { print $val0; } ?>" />
                           <input type="button" class="short" value="+" onclick="javascript:addblock('', '', '<?php print $text_from; ?>', '<?php print $text_to; ?>', '<?php print $text_subject; ?>', '<?php print $text_body; ?>'); return false;" />
                           <input type="button" class="short" value="-" onclick="javascript:removeme('myid0'); return false;" />
                  </div>
         </div>
</div>

      </div>


      <div id="ss1" style="border: 0px solid blue;">
         <div class="row">
            <div class="cell1" style="border: 0px solid blue; width: 87px;"><?php print $text_date; ?>: </div>
            <div class="cell2"><input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" /> <input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" style="width: 170px;"/></div>
         </div>
      </div>

      <div id="ss1" style="border: 0px solid blue;">
         <div class="row">
            <div class="cell1s"><?php print $text_direction; ?>: </div>
            <div class="cell2">
               <select name="direction" id="direction" class="advselect2">
                  <option value=""><?php print $text_any; ?></option>
                  <option value="0"<?php if(isset($direction) && $direction == 0) { ?> selected="selected"<?php } ?>><?php print $text_inbound; ?></option>
                  <option value="2"<?php if(isset($direction) && $direction == 2) { ?> selected="selected"<?php } ?>><?php print $text_outbound; ?></option>
                  <option value="1"<?php if(isset($direction) && $direction == 1) { ?> selected="selected"<?php } ?>><?php print $text_internal; ?></option>
               </select>
            </div>
         </div>
      </div>

      <div id="ss1" style="border: 0px solid blue;">
         <div class="row">
            <div class="cell1s"><?php print $text_attachment_type; ?>: </div>
            <div class="cell2">
               <div id="atypes" style="display: table;">
                  <div class="row">
                     <div class="cell5"><input type="checkbox" name="attachment_type_word" class="attachmenttype" id="attachment_type_word" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/doc.png" /> word</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_excel" class="attachmenttype" id="attachment_type_excel" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/xls.png" /> excel</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_powerpoint" class="attachmenttype" id="attachment_type_powerpoint" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/ppt.png" alt="powerpoint" /> powerpoint</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_pdf" class="attachmenttype" id="attachment_type_pdf" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/pdf.png" /> pdf</div>
                  </div>
                  <div class="row">
                     <div class="cell5"><input type="checkbox" name="attachment_type_compressed" class="attachmenttype" id="attachment_type_compressed" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/rar.png" /> <?php print $text_compressed; ?></div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_text" class="attachmenttype" id="attachment_type_text" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/txt.png" /> <?php print $text_text2; ?></div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_odf" class="attachmenttype" id="attachment_type_odf" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/odf.png" /> odf</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_image" class="attachmenttype" id="attachment_type_image" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/image.png" /> <?php print $text_image; ?></div>
                  </div>
                  <div class="row">
                     <div class="cell5"><input type="checkbox" name="attachment_type_audio" class="attachmenttype" id="attachment_type_audio" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/audio.png" /> audio</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_video" class="attachmenttype" id="attachment_type_video" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/video.png" /> video</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_flash" class="attachmenttype" id="attachment_type_flash" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/swf.png" /> flash</div>
                     <div class="cell5"><input type="checkbox" name="attachment_type_other" class="attachmenttype" id="attachment_type_other" /> <img src="/view/theme/<?php print THEME; ?>/images/fileicons/file.png" /> <?php print $text_other; ?></div>
                  </div>
               </div>
            </div>
         </div>
      </div>

      <div class="row">
         <div class="cell1s">&nbsp;</div>
         <div class="cell2">
            <button id="button_search" class="active" onclick="script:load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0);"><?php print $text_search; ?></button>
            <input type="button" class="advsecondary" onclick="javascript:reset_adv_form(count); return false;" value="<?php print $text_cancel; ?>" />
            <input type="button" class="advsecondary" value="<?php print $text_save; ?>" onclick="javascript:send_ajax_post_request('<?php print SAVE_SEARCH_URL; ?>', assemble_search_term(count) + '&save=1', 'A1');" />
            <input type="button" class="load" name="load" onclick="javascript:load_ajax_url('<?php print SAVE_SEARCH_URL; ?>&<?php if(isset($search_args)) { print $search_args; } ?>');" value="<?php print $text_load; ?>..." />
         </div>
      </div>


         <input type="hidden" name="tag" value="" />


   </div>

   <div id="search2">
      <div id="A1"><a href="expert.php"><?php print $text_expert_search; ?></a></div>
   </div>

</div>


<div id="AS">
   <div id="resultscontainer" class="empty"></div>
</div>


<?php if(isset($searchterm)) { ?>

   <script>
<?php foreach($blocks as $block) { ?>
      addblock('<?php print $block['key']; ?>', '<?php print $block['val']; ?>', '<?php print $text_from; ?>', '<?php print $text_to; ?>', '<?php print $text_subject; ?>', '<?php print $text_body; ?>');
<?php } ?>

      fix_search_order('<?php print $sort; ?>', '<?php print $order; ?>');

      load_search_results('<?php print SEARCH_HELPER_URL; ?>', assemble_search_term(count), 0);
      load_ajax_url('<?php print SAVE_SEARCH_URL; ?>');

   </script>

<?php } ?>

