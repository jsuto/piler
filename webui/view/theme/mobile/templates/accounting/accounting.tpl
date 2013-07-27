<p>&nbsp;</p>

<p>
<?php if ($view == 'email') { echo '<strong>'.$text_accounting_email.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=email">'.$text_accounting_email.'</a>'; } ?> | 
<?php if ($view == 'domain') { echo '<strong>'.$text_accounting_domain.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=domain">'.$text_accounting_domain.'</a>'; } ?>
</p>


<?php if ( $accounting ) { ?>

    <div id="pagenav">
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=0&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $prev_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
        <?php if(count($accounting) > 0) { print $accounting[0]['display']; ?> - <?php print $accounting[count($accounting)-1]['display']; } ?>
        <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $next_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?></a><?php } ?>
        <?php if($page < $total_pages){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $total_pages; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
    </div>

    <div id="ss1" style="margin-top: 10px;">

        <div class="domainrow">
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell"><?php print $text_sent; ?></div>
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell"><?php print $text_received; ?></div>
            <div class="domaincell">&nbsp;</div>
            <div class="domaincell">&nbsp;</div>
        </div>
        <div class="domainrow">
            <div class="domaincell"><?php echo $viewname; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=item&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=item&amp;order=1"><i class="icon-chevron-down"></i></a></div>
        <?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>
            <div class="domaincell"><?php print $text_users; ?></div>
        <?php } ?>
            <div class="domaincell"><?php print $text_oldest_record; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=oldest&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=oldest&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_newest_record; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=newest&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=newest&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_items; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sent&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sent&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentsize&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentsize&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_average_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentavg&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentavg&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_items; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recd&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recd&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdsize&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdsize&amp;order=1"><i class="icon-chevron-down"></i></a></div>
            <div class="domaincell"><?php print $text_average_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdavg&amp;order=0"><i class="icon-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdavg&amp;order=1"><i class="icon-chevron-down"></i></a></div>
        </div>


        <?php foreach($accounting as $details) {?>
            <div class="domainrow">
                <div class="domaincell"><?php echo $details['item']; ?></div>

             <?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>
                <div><a href="index.php?route=accounting/accounting&amp;view=accounts&amp;domain=<?php echo $details['item']; ?>"><?php echo count($accounts[$details['item']]); ?></a></div>
             <?php } ?>

                <div class="domaincell"><?php echo date("d M Y",$details['oldest']); ?></div>
                <div class="domaincell"><?php echo date("d M Y",$details['newest']); ?></div>
                <div class="domaincell">
                    <?php echo $details['sent']; ?>
                </div>
                <div class="domaincell">
                    <?php echo nice_size($details['sentsize']); ?>
                </div>
                <div class="domaincell">
                    <?php echo nice_size($details['sentavg']); ?>
                </div>
                <div class="domaincell">
                    <?php echo $details['recd']; ?>
                </div>
                <div class="domaincell">
                    <?php echo nice_size($details['recdsize']); ?>
                </div>
                <div class="domaincell">
                    <?php echo nice_size($details['recdavg']); ?>
                </div>
            </div>
        <?php } ?>

    </div>


    <div id="pagenav">
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=0&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &laquo; <?php if($page > 0){ ?></a><?php } ?>
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $prev_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &lsaquo; <?php if($page > 0){ ?></a><?php } ?>
        <?php if(count($accounting) > 0) { print $accounting[0]['display']; ?> - <?php print $accounting[count($accounting)-1]['display']; } ?>
        <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $next_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &rsaquo; <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?></a><?php } ?>
        <?php if($page < $total_pages){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $total_pages; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?> &raquo; <?php if($page < $total_pages){ ?></a><?php } ?>
    </div>
<?php } else { ?>
    <p><?php $s = 'text_no_' . $view . '_found'; print $$s; ?></p>
<?php } ?>

