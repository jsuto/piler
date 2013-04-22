   <div id="searchpopup1" class="container">

        <input type="hidden" name="xsearchtype" id="xsearchtype" value="simple" />

        <input type="hidden" name="xsort" id="xsort" value="date" />
        <input type="hidden" name="xorder" id="xorder" value="0" />
        <input type="hidden" name="xref" id="xref" value="" />

        <div id="popupx" onclick="$('#searchpopup1').hide();"><i class="icon-remove-circle icon-large"></i></div>
        
        <form id="ss1" class="form-horizontal">

            <div class="control-group">
                <label class="control-label" for="xfrom"><?php print $text_from; ?>:</label>
                <div class="controls">
                  <input type="text" name="xfrom" id="xfrom" value="<?php if(isset($from)) { print $from; } ?>" />
                </div>
            </div>

            <div class="control-group">
                <label class="control-label" for="xto"><?php print $text_to; ?>:</label>
                <div class="controls">
                  <input type="text" name="xto" id="xto" value="<?php if(isset($to)) { print $to; } ?>" />
                </div>
            </div>

            <div class="control-group">
                <label class="control-label" for="xsubject"><?php print $text_subject; ?>:</label>
                <div class="controls">
                  <input type="text" name="xsubject" id="xsubject" value="<?php if(isset($subject)) { print $subject; } ?>" />
                </div>
            </div>            
            
            <div class="control-group">
                <label class="control-label" for="xbody"><?php print $text_body; ?>:</label>
                <div class="controls">
                  <input type="text" name="xbody" id="xbody" value="<?php if(isset($body)) { print $body; } ?>" />
                </div>
            </div>
            
            <div class="control-group">
                <label class="control-label" for="xtag"><?php print $text_tags; ?>:</label>
                <div class="controls">
                  <input type="text" name="xtag" id="xtag" value="<?php if(isset($tag)) { print $tag; } ?>" />
                </div>
            </div>            

            <div class="control-group">
                <label class="control-label" for="xnote"><?php print $text_notes; ?>:</label>
                <div class="controls">
                  <input type="text" name="xnote" id="xnote" value="<?php if(isset($note)) { print $note; } ?>" />
                </div>
            </div>
            
            <div class="control-group">
                <label class="control-label" for="xhas_attachment"><?php print $text_attachment; ?>:</label>
                <div class="controls">
                  <input type="checkbox" name="xhas_attachment" id="xhas_attachment" <?php if(isset($has_attachment) && $has_attachment == 1) { ?>checked="checked"<?php } ?> />
                </div>
            </div> 

            <div class="control-group">
                <label class="control-label" for="date1"><?php print $text_from; ?> <?php print $text_date; ?>:</label>
                <div class="controls">
                  <input type="text" name="date1" id="date1" size="11" value="<?php if(isset($date1)) { print $date1; } ?>" />
                </div>
            </div>            
            
            <div class="control-group">
                <label class="control-label" for="date2"><?php print $text_to; ?> <?php print $text_date; ?>:</label>
                <div class="controls">
                  <input type="text" name="date2" id="date2" size="11" value="<?php if(isset($date2)) { print $date2; } ?>" />
                </div>
            </div>

             <div class="form-actions">
                <input type="submit" id="button_search" class="btn btn-danger" onclick="Piler.complex();return false;" value="<?php print $text_search; ?>" />
                <input type="button" class="btn" onclick="Piler.cancel();" value="<?php print $text_clear; ?>" />
             </div>
             

       </form>

   </div>

