   <div id="searchpopup1">

        <input type="hidden" name="xsearchtype" id="xsearchtype" value="simple" />

        <input type="hidden" name="xsort" id="xsort" value="date" />
        <input type="hidden" name="xorder" id="xorder" value="0" />
        <input type="hidden" name="xref" id="xref" value="" />

        <div id="popupx" onclick="$('#searchpopup1').hide();"> <i class="icon-remove"></i> </div>

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
            <div class="cell2 left">
               <img src="view/theme/mobile/images/fileicons/doc.png" alt="Word" title="Word" /> <input type="checkbox" class="checkbox popup" name="xhas_attachment_doc" id="xhas_attachment_doc" <?php if(isset($has_attachment_doc) && $has_attachment_doc == 1) { ?>checked="checked"<?php } ?> />
               <img src="view/theme/mobile/images/fileicons/xls.png" alt="Excel" title="Excel" /> <input type="checkbox" class="checkbox popup" name="xhas_attachment_xls" id="xhas_attachment_xls" <?php if(isset($has_attachment_xls) && $has_attachment_xls == 1) { ?>checked="checked"<?php } ?> />
               <img src="view/theme/mobile/images/fileicons/pdf.png" alt="PDF" title=PDF"" /> <input type="checkbox" class="checkbox popup" name="xhas_attachment_pdf" id="xhas_attachment_pdf" <?php if(isset($has_attachment_pdf) && $has_attachment_pdf == 1) { ?>checked="checked"<?php } ?> />
               <img src="view/theme/mobile/images/fileicons/image.png" alt="image" title="image" /> <input type="checkbox" class="checkbox popup" name="xhas_attachment_image" id="xhas_attachment_image" <?php if(isset($has_attachment_image) && $has_attachment_image == 1) { ?>checked="checked"<?php } ?> />
               <img src="view/theme/mobile/images/fileicons/file.png" alt="any" title="any" /> <input type="checkbox" class="checkbox popup" name="xhas_attachment_any" id="xhas_attachment_any" <?php if(isset($has_attachment_any) && $has_attachment_any == 1) { ?>checked="checked"<?php } ?> />
            </div>
         </div>

         <div class="row">
            <div class="cell1"><?php print $text_date; ?>: </div>
            <div class="cell2"><input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" placeholder="<?php print DATE_FORMAT; ?>" /></div>
         </div>

         <div class="row">
            <div class="cell1">&nbsp;</div>
            <div class="cell2"><input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" placeholder="<?php print DATE_FORMAT; ?>" /></div>
         </div>  

         <div class="row">
            <div class="cell1"></div>
            <div class="cell2 left">
               <button id="button_search" class="btn btn-danger" onclick="Piler.complex();"><?php print $text_search; ?></button> <input type="button" class="btn" onclick="Piler.cancel();" value="<?php print $text_cancel; ?>" />
            </div>
         </div>


       </div>

   </div>

