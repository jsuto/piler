
<h4><?php print $text_add_new_domain; ?></h4>

<form method="post" name="add1" action="index.php?route=domain/domain" class="formbottom">

   <div id="ss1">
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
         <div class="domaincell"><input type="submit" class="btn btn-primary" value="<?php print $text_add; ?>" /> <input type="reset" class="btn" value="<?php print $text_cancel; ?>" /></div>
      </div>

   </div>

</form>

<h4><?php print $text_existing_domains; ?></h4>

<div class="listarea">

<?php if(isset($domains)){ ?>

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_domain; ?></div>
         <div class="domaincell"><?php print $text_mapped_domain; ?></div>
         <div class="domaincell">&nbsp;</div>
      </div>

<?php foreach($domains as $domain) { ?>
      <div class="domainrow">
         <div class="domaincell"><a href="index.php?route=user/list&search=@<?php print $domain['domain']; ?>"><?php print $domain['domain']; ?></a></div>
         <div class="domaincell"><?php print $domain['mapped']; ?></div>
         <div class="domaincell"><a href="index.php?route=domain/remove&amp;confirmed=1&amp;domain=<?php print urlencode($domain['domain']); ?>" onclick="if(confirm('<?php print $text_remove_domain; ?>: ' + '\'<?php print $domain['domain']; ?>\'')) return true; return false;"><?php print $text_remove; ?></a></div>
      </div>
<?php } ?>

   </div>

<?php } else { ?>
<?php print $text_not_found; ?>
<?php } ?>

</div>

