<form method="post" name="search1" action="index.php?route=accounting/accounting&view=<?php echo $view; ?>" class="form-inline pull-right">
    <div class="input-append">
        <input type="text" name="search" class="input-medium" value="<?php print $search; ?>" />
        <input type="submit" class="btn" value="<?php print $text_search; ?>" />
    </div>
</form>

<p>
<?php if ($view == 'email') { echo '<strong>'.$text_accounting_email.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=email">'.$text_accounting_email.'</a>'; } ?> | 
<?php if ($view == 'domain') { echo '<strong>'.$text_accounting_domain.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=domain">'.$text_accounting_domain.'</a>'; } ?>
</p>

<?php if ( $accounting ) { ?>

    <div class="pagenav container">
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=0&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left icon-large"></i><?php if($page > 0){ ?></a><?php } ?>
        &nbsp;
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $prev_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left icon-large"></i><?php if($page > 0){ ?></a><?php } ?>
        &nbsp;
        <?php if(count($accounting) > 0) { print $accounting[0]['display']; ?> - <?php print $accounting[count($accounting)-1]['display']; } ?>
        &nbsp;
        <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $next_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right icon-large"></i><?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?></a><?php } ?>
        &nbsp;
        <?php if($page < $total_pages){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $total_pages; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right icon-large"></i><?php if($page < $total_pages){ ?></a><?php } ?>
    </div>

    <table class="table table-striped table-condensed">

        <tr>
            <th colspan="3">&nbsp;</th>
            <th colspan="3"><?php print $text_sent; ?></th>
            <th colspan="3"><?php print $text_received; ?></th>
        </tr>
        <tr>
            <th><?php echo $viewname; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=item&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=item&amp;order=1"><i class="icon-chevron-down"></i></a></th>

        <?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>
            <th><?php print $text_users; ?></th>
        <?php } ?>

            <th><?php print $text_oldest_record; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=oldest&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=oldest&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_newest_record; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=newest&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=newest&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_items; ?>  <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sent&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sent&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentsize&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentsize&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_average_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentavg&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentavg&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_items; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recd&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recd&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdsize&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdsize&amp;order=1"><i class="icon-chevron-down"></i></a></th>
            <th><?php print $text_average_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdavg&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdavg&amp;order=1"><i class="icon-chevron-down"></i></a></th>
        </tr>
        
        <?php foreach($accounting as $details) {?>
        <tr>
            <td><?php echo $details['item']; ?></td>

        <?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>
            <td><a href="index.php?route=accounting/accounting&amp;view=accounts&amp;domain=<?php echo $details['item']; ?>"><?php echo count($accounts[$details['item']]); ?></a></td>
        <?php } ?>

            <td><?php echo date(DATE_TEMPLATE, $details['oldest']); ?></td>
            <td><?php echo date(DATE_TEMPLATE, $details['newest']); ?></td>
            <td>
                <?php echo $details['sent']; ?>
            </td>
            <td>
                <?php echo nice_size($details['sentsize']); ?>
            </td>
            <td>
                <?php echo nice_size($details['sentavg']); ?>
            </td>
            <td>
                <?php echo $details['recd']; ?>
            </td>
            <td>
                <?php echo nice_size($details['recdsize']); ?>
            </td>
            <td>
                <?php echo nice_size($details['recdavg']); ?>
            </td>
        </tr>
        <?php } ?>
    </table>
    
    <div class="pagenav container">
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=0&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-left icon-large"></i><?php if($page > 0){ ?></a><?php } ?>
        &nbsp;
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $prev_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-left icon-large"></i><?php if($page > 0){ ?></a><?php } ?>
        &nbsp;
        <?php if(count($accounting) > 0) { print $accounting[0]['display']; ?> - <?php print $accounting[count($accounting)-1]['display']; } ?>
        &nbsp;
        <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $next_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-angle-right icon-large"></i><?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?></a><?php } ?>
        &nbsp;
        <?php if($page < $total_pages){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $total_pages; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="icon-double-angle-right icon-large"></i><?php if($page < $total_pages){ ?></a><?php } ?>
    </div>
    
<?php } else { ?>
<tr>
    <td colspan='6'><?php print $text_no_email_found; ?></td>
</tr>
<?php } ?>
