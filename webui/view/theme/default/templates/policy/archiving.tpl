
<h4><?php print $text_add_new_rule; ?></h4>

<form method="post" name="add1" action="index.php?route=policy/archiving" style="margin-bottom: 30px;">

   <div id="ss1" style="margin-top: 10px; width: 600px; border: 1px solid red;">

      <div class="row">
         <div class="domaincell"><?php print $text_from; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="from" /></div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_to; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="to" /></div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_subject; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="subject" /></div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_size; ?>:</div>
         <div class="domaincell">
            <select class="ruleselect" name="_size">
               <option value=">">&gt;</option>
               <option value="=">=</option>
               <option value="<">&lt;</option>
            </select>
            <input type="text" class="ruletext" name="size" />
         </div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_attachment_type; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="attachment_type" /></div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_attachment_size; ?>:</div>
         <div class="domaincell">
            <select class="ruleselect" name="_attachment_size">
               <option value=">">&gt;</option>
               <option value="=">=</option>
               <option value="<">&lt;</option>
            </select>
            <input type="text" class="ruletext" name="attachment_size" />
         </div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_spam; ?>:</div>
         <div class="domaincell">
            <select class="ruleselect" name="spam">
               <option value="-1">-</option>
               <option value="0"><?php print $text_not_spam; ?></option>
               <option value="1"><?php print $text_spam2; ?></option>
            </select>
            <input type="text" class="ruletext" name="attachment_size" />
         </div>
      </div>


      <div class="row">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_add; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>

</form>


<h4><?php print $text_existing_rules; ?></h4>

<?php if(isset($rules)){ ?>

   <div id="ss1" style="margin-top: 10px; border: 1px solid red;">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_from; ?></div>
         <div class="domaincell"><?php print $text_to; ?></div>
         <div class="domaincell"><?php print $text_subject; ?></div>
         <div class="domaincell"><?php print $text_spam; ?></div>
         <div class="domaincell"><?php print $text_size; ?></div>
         <div class="domaincell"><?php print $text_attachment_type; ?></div>
         <div class="domaincell"><?php print $text_attachment_size; ?></div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($rules as $rule) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $rule['from']; ?></div>
         <div class="domaincell"><?php print $rule['to']; ?></div>
         <div class="domaincell"><?php print $rule['subject']; ?></div>
         <div class="domaincell"><?php if($rule['spam'] == -1) { print "-"; } else if($rule['spam'] == 0) { print $text_not_spam; } else { print $text_spam; } ?></div>
         <div class="domaincell"><?php if($rule['size'] > 0) { print $rule['_size']; ?> <?php print $rule['size']; } ?></div>
         <div class="domaincell"><?php print $rule['attachment_type']; ?></div>
         <div class="domaincell"><?php if($rule['attachment_size'] > 0) { print $rule['_attachment_size']; ?> <?php print $rule['attachment_size']; } ?></div>
         <div class="domaincell"><a href="index.php?route=policy/removearchiving&amp;id=<?php print $rule['id']; ?>"><?php print $text_remove; ?></a></div>
      </div>
<?php } ?>

   </div>

<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>


