
<p>
<?php if ($view == 'email') { echo '<strong>'.$text_accounting_email.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=email">'.$text_accounting_email.'</a>'; } ?> | 
<?php if ($view == 'domain') { echo '<strong>'.$text_accounting_domain.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=domain">'.$text_accounting_domain.'</a>'; } ?>
</p>

<p><a href="index.php?route=accounting/accounting&amp;view=domain"><?php print $text_back; ?></a></p>

<?php if(count($accounts) > 0) { ?>

    <div id="ss1">

        <div class="domainrow">
            <div class="domaincell"><?php echo $text_email; ?> (@<?php print $domain; ?>) </div>
        </div>
        
        <?php foreach($accounts as $a) {?>
        <div class="domainrow">
            <div class="domaincell"><?php echo $a['mail']; ?></div>
        </div>
        <?php } ?>
    </div>
    
<?php } else { ?>
<div class="domainrow">
    <div class="domaincell"><?php print $text_no_email_found; ?></div>
</div>
<?php } ?>

<p><a href="index.php?route=accounting/accounting&amp;view=domain"><?php print $text_back; ?></a></p>


