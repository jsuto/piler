
<h4><?php print $text_add_new_domain; ?></h4>

<form method="post" name="add1" action="index.php?route=domain/domain" style="margin-bottom: 30px;">

   <div id="ss1" style="margin-top: 10px;">
      <div class="row">
         <div class="domaincell"><?php print $text_domain; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="domain" /></div>
      </div>
      <div class="row">
         <div class="domaincell"><?php print $text_mapped_domain; ?>:</div>
         <div class="domaincell"><input type="text" class="text" name="mapped" /></div>
      </div>
      <div class="row">
         <div class="domaincell">&nbsp;</div>
         <div class="domaincell"><input type="submit" value="<?php print $text_add; ?>" /> <input type="reset" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>

</form>


<h4><?php print $text_existing_domains; ?></h4>

<?php if(isset($domains)){ ?>

   <div id="ss1" style="margin-top: 10px;">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_domain; ?></div>
         <div class="domaincell"><?php print $text_mapped_domain; ?></div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($domains as $domain) { ?>
      <div class="domainrow">
         <div class="domaincell"><a href="index.php?route=user/list&search=@<?php print $domain['domain']; ?>"><?php print $domain['domain']; ?></a></div>
         <div class="domaincell"><?php print $domain['mapped']; ?></div>
         <div class="domaincell"><a href="index.php?route=domain/remove&amp;domain=<?php print urlencode($domain['domain']); ?>"><?php print $text_remove; ?></a></div>
      </div>
<?php } ?>

   </div>

<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>


