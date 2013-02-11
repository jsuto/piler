   <div id="searchpopup1">

        <input type="hidden" name="xsearchtype" id="xsearchtype" value="simple" />

        <input type="hidden" name="xsort" id="xsort" value="date" />
        <input type="hidden" name="xorder" id="xorder" value="0" />
        <input type="hidden" name="xref" id="xref" value="" />

        <div id="popupx" onclick="$('#searchpopup1').hide();"> X </div>

        <div id="ss1">

         <div class="row">
            <div class="cell1"></div>
            <div class="cell2"></div>
         </div>  

         <div class="row">
            <div class="cell1"><?php print $text_from; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="xfrom" id="xfrom" value="<?php if(isset($from)) { print $from; } ?>" /></div>
         </div>  

         <div class="row">
            <div class="cell1"><?php print $text_to; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="xto" id="xto" value="<?php if(isset($to)) { print $to; } ?>" /></div>
         </div>  

         <div class="row">
            <div class="cell1"><?php print $text_subject; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="xsubject" id="xsubject" value="<?php if(isset($subject)) { print $subject; } ?>" /></div>
         </div>  

         <div class="row">
            <div class="cell1"><?php print $text_body; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="xbody" id="xbody" value="<?php if(isset($body)) { print $body; } ?>" /></div>
         </div>  

         <div class="row">
            <div class="cell1"><?php print $text_tags; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="xtag" id="xtag" value="<?php if(isset($tag)) { print $tag; } ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_notes; ?>:</div>
            <div class="cell2"><input type="text" class="text" name="xnote" id="xnote" value="<?php if(isset($note)) { print $note; } ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_attachment; ?>:</div>
            <div class="cell2 left"><input type="checkbox" class="checkbox popup" name="xhas_attachment" id="xhas_attachment" <?php if(isset($has_attachment) && $has_attachment == 1) { ?>checked="checked"<?php } ?> /></div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_date; ?>: </div>
            <div class="cell2"><input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" /> <input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" /></div>
         </div>  

         <div class="row">
            <div class="cell1"></div>
            <div class="cell2 left">
               <button id="button_search" class="active popup" onclick="Piler.complex();"><?php print $text_search; ?></button> <input type="button" class="advsecondary_complex" onclick="Piler.cancel();" value="<?php print $text_cancel; ?>" />
            </div>
         </div>


       </div>

   </div>

