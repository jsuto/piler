<div class="container text-start">
  <div class="row">
    <div class="col">
<p>
<?php if ($view == 'email') { echo '<strong>'.$text_accounting_email.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=email">'.$text_accounting_email.'</a>'; } ?> |
<?php if ($view == 'domain') { echo '<strong>'.$text_accounting_domain.'</strong>'; } else { echo '<a href="index.php?route=accounting/accounting&amp;view=domain">'.$text_accounting_domain.'</a>'; } ?>
</p>

    </div>
    <div class="col text-end">
      <form method="post" name="search1" action="index.php?route=accounting/accounting&view=<?php echo $view; ?>">
        <div class="row justify-content-end">
          <div class="col-8">
            <input type="text" name="search" class="form-control" value="<?php print $search; ?>">
          </div>
          <div class="col-2">
            <button type="submit" class="btn btn-primary"><?php print $text_search; ?></button>
          </div>
        </div>
      </form>
    </div>
  </div>
</div>


<?php if($accounting) { ?>

    <table class="table table-striped mt-3">
      <thead class="table-secondary">
        <tr>
            <th colspan="<?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>5<?php } else { ?>4<?php } ?>">&nbsp;</th>
            <th colspan="3"><?php print $text_sent; ?></th>
            <th colspan="3"><?php print $text_received; ?></th>
        </tr>
        <tr>
            <th class="text-start"><?php echo $viewname; ?> <?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>(<?php print $mydomain_count; ?>)<?php } ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=item&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=item&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>

        <?php if(ENABLE_SAAS == 1 && $view == 'domain') { ?>
            <th><?php print $text_users; ?> (<?php print $user_count; ?>)</th>
        <?php } ?>

            <th><?php print $text_oldest_record; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=oldest&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=oldest&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_newest_record; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=newest&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=newest&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_items; ?>  <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sent&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sent&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentsize&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentsize&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_average_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentavg&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=sentavg&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_items; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recd&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recd&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdsize&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdsize&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
            <th><?php print $text_average_size; ?> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdavg&amp;order=0"><i class="bi bi-chevron-up"></i></a> <a href="index.php?route=accounting/accounting&amp;view=<?php echo $view; ?>&amp;sort=recdavg&amp;order=1"><i class="bi bi-chevron-down"></i></a></th>
        </tr>
      </thead>
      <tbody>
        <?php foreach($accounting as $details) {?>
        <tr>
            <td class="text-start"><?php echo $details['item']; ?></td>

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
      </tbody>
    </table>

  <?php if($total_records > $page_len) { ?>
    <div class="pagenav container">
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=0&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-double-left"></i><?php if($page > 0){ ?></a><?php } ?>
        &nbsp;
        <?php if($page > 0){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $prev_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-left"></i><?php if($page > 0){ ?></a><?php } ?>
        &nbsp;
        <?php if(count($accounting) > 0) { print $accounting[0]['display']; ?> - <?php print $accounting[count($accounting)-1]['display']; } ?>
        &nbsp;
        <?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $next_page; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-right"></i><?php if($total_records >= $page_len*($page+1) && $total_records > $page_len){ ?></a><?php } ?>
        &nbsp;
        <?php if($page < $total_pages){ ?><a href="index.php?route=accounting/accounting&amp;page=<?php print $total_pages; ?>&amp;view=<?php echo $view; ?>&amp;sort=<?php print $sort; ?>&amp;order=<?php print $order; ?>" class="navlink"><?php } ?><i class="bi bi-chevron-double-right"></i><?php if($page < $total_pages){ ?></a><?php } ?>
    </div>
  <?php } ?>

<?php } else { ?>
<tr>
    <td colspan='6'><?php print $text_no_email_found; ?></td>
</tr>
<?php } ?>
