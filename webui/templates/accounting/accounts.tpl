
<p>
<?php if ($view == 'email') { echo '<strong>'.$text_accounting_email.'</strong>'; } else { echo '<a href="<?php print PATH_PREFIX; ?>index.php?route=accounting/accounting&amp;view=email">'.$text_accounting_email.'</a>'; } ?> |
<?php if ($view == 'domain') { echo '<strong>'.$text_accounting_domain.'</strong>'; } else { echo '<a href="<?php print PATH_PREFIX; ?>index.php?route=accounting/accounting&amp;view=domain">'.$text_accounting_domain.'</a>'; } ?>
</p>

<p><a href="<?php print PATH_PREFIX; ?>index.php?route=accounting/accounting&amp;view=domain"><?php print $text_back; ?></a></p>

<?php if(count($accounts) > 0) { ?>

    <table class="table table-striped table-condensed">

        <tr>
            <th><?php echo $text_email; ?> (@<?php print $domain; ?>) </th>
        </tr>

        <?php foreach($accounts as $a) {?>
        <tr>
            <td><?php echo $a['mail']; ?></td>
        </tr>
        <?php } ?>
    </table>

<?php } else { ?>
<tr>
    <td><?php print $text_no_email_found; ?></td>
</tr>
<?php } ?>

<p><a href="<?php print PATH_PREFIX; ?>index.php?route=accounting/accounting&amp;view=domain"><?php print $text_back; ?></a></p>
