
<div id="search">
   <div id="search1">

      <div id="ss1" style="border: 0px solid red;">

         <input type="hidden" name="sort" id="sort" value="date" />
         <input type="hidden" name="order" id="order" value="0" />

<div id="myid">

         <div id="myid0" class="row" style="margin:0; padding:0; border: 0px solid blue; float: left;">

                  <div class="cell1s">
                           <select name="f[]" id="key0" class="advselect">
                              <option value="user"<?php if(isset($key0) && $key0 == 0) { ?> selected="selected"<?php } ?>><?php print $text_user; ?></option>
                              <option value="ipaddr"<?php if(isset($key0) && $key0 == 1) { ?> selected="selected"<?php } ?>><?php print $text_ipaddr; ?></option>
                              <option value="ref"<?php if(isset($key0) && $key0 == 2) { ?> selected="selected"<?php } ?>><?php print $text_ref; ?></option>
                           </select>
                  </div>
                  <div class="cell2">
                           <input type="text" class="advtext" name="v[]" id="val0" value="<?php if(isset($val0)) { print $val0; } ?>" />
                           <input type="button" class="short" value="+" onclick="javascript:addauditblock('', '', '<?php print $text_user; ?>', '<?php print $text_ipaddr; ?>', '<?php print $text_ref; ?>'); return false;" />
                           <input type="button" class="short" value="-" onclick="javascript:removeme('myid0'); return false;" />
                  </div>
         </div>
</div>

      </div>


      <div id="ss1" style="border: 0px solid blue;">
         <div class="row">
            <div class="cell1" style="border: 0px solid blue; width: 87px;"><?php print $text_date; ?>: </div>
            <div class="cell2"><input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" />&nbsp; &nbsp;<input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" style="width: 170px;"/></div>
         </div>
      </div>

      <div id="ss1" style="border: 0px solid blue;">
         <div class="row">
            <div class="cell1s"><?php print $text_action; ?>: </div>
            <div class="cell2">
               <select name="action" id="action" class="advselect2">
                  <option value="<?php print ACTION_ALL; ?>"><?php print $text_all; ?></option>
                  <option value="<?php print ACTION_UNKNOWN; ?>"<?php if(isset($action) && $action == ACTION_UNKNOWN) { ?> selected="selected"<?php } ?>><?php print $text_unknown; ?></option>
                  <option value="<?php print ACTION_LOGIN; ?>"<?php if(isset($action) && $action == ACTION_LOGIN) { ?> selected="selected"<?php } ?>><?php print $text_login2; ?></option>
                  <option value="<?php print ACTION_LOGIN_FAILED; ?>"<?php if(isset($action) && $action == ACTION_LOGIN_FAILED) { ?> selected="selected"<?php } ?>><?php print $text_login_failed; ?></option>
                  <option value="<?php print ACTION_LOGOUT; ?>"<?php if(isset($action) && $action == ACTION_LOGOUT) { ?> selected="selected"<?php } ?>><?php print $text_logout2; ?></option>
                  <option value="<?php print ACTION_VIEW_MESSAGE; ?>"<?php if(isset($action) && $action == ACTION_VIEW_MESSAGE) { ?> selected="selected"<?php } ?>><?php print $text_view_message2; ?></option>
                  <option value="<?php print ACTION_VIEW_HEADER; ?>"<?php if(isset($action) && $action == ACTION_VIEW_HEADER) { ?> selected="selected"<?php } ?>><?php print $text_view_header; ?></option>
                  <option value="<?php print ACTION_UNAUTHORIZED_VIEW_MESSAGE; ?>"<?php if(isset($action) && $action == ACTION_UNAUTHORIZED_VIEW_MESSAGE) { ?> selected="selected"<?php } ?>><?php print $text_unauthorized_view_message; ?></option>
                  <option value="<?php print ACTION_RESTORE_MESSAGE; ?>"<?php if(isset($action) && $action == ACTION_RESTORE_MESSAGE) { ?> selected="selected"<?php } ?>><?php print $text_restore_message; ?></option>
                  <option value="<?php print ACTION_DOWNLOAD_MESSAGE; ?>"<?php if(isset($action) && $action == ACTION_DOWNLOAD_MESSAGE) { ?> selected="selected"<?php } ?>><?php print $text_download_message2; ?></option>
                  <option value="<?php print ACTION_SEARCH; ?>"<?php if(isset($action) && $action == ACTION_SEARCH) { ?> selected="selected"<?php } ?>><?php print $text_search2; ?></option>
                  <option value="<?php print ACTION_SAVE_SEARCH; ?>"<?php if(isset($action) && $action == ACTION_SAVE_SEARCH) { ?> selected="selected"<?php } ?>><?php print $text_save_search; ?></option>
                  <option value="<?php print ACTION_CHANGE_USER_SETTINGS; ?>"<?php if(isset($action) && $action == ACTION_CHANGE_USER_SETTINGS) { ?> selected="selected"<?php } ?>><?php print $text_change_user_settings; ?></option>
                  <option value="<?php print ACTION_REMOVE_MESSAGE; ?>"<?php if(isset($action) && $action == ACTION_REMOVE_MESSAGE) { ?> selected="selected"<?php } ?>><?php print $text_remove_message2; ?></option>
                  <option value="<?php print ACTION_UNAUTHORIZED_REMOVE_MESSAGE; ?>"<?php if(isset($action) && $action == ACTION_UNAUTHORIZED_REMOVE_MESSAGE) { ?> selected="selected"<?php } ?>><?php print $text_unauthorized_remove_message; ?></option>
               </select>
            </div>
         </div>
      </div>

      <div class="row">
         <div class="cell1s">&nbsp;</div>
         <div class="cell2">
            <button id="button_search" class="active" onclick="script:load_search_results('<?php print AUDIT_HELPER_URL; ?>', assemble_search_term(count), 0);"><?php print $text_search; ?></button>
         </div>
      </div>


         <input type="hidden" name="tag" value="" />


   </div>

   <div id="search2">
      <div id="A1"></div>
   </div>

</div>


<div id="AS">
   <div id="auditresultscontainer" class="empty">
   </div>
</div>


<?php if(isset($searchterm)) { ?>

   <script>
<?php foreach($blocks as $block) { ?>
      addauditblock('<?php print $block['key']; ?>', '<?php print $block['val']; ?>', '<?php print $text_user; ?>', '<?php print $text_ipaddr; ?>', '<?php print $text_ref; ?>');
<?php } ?>

   </script>

<?php } ?>

