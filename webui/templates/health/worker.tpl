<div class="container text-start">
  <div class="row">
    <div class="col mb-5 d-flex align-items-center" style="background: #d9edf7; color: #3a87ad; height: 40px;">
      <strong><?php print $text_refresh_period; ?>:</strong>&nbsp;<?php print HEALTH_REFRESH; ?> sec
    </div>
  </div>
  <div class="row">
    <div class="col-8"> <!-- left side -->

      <div class="container text-start">
        <div class="row">
          <div class="col">
            <h2><i class="bi bi-speedometer2"></i> <?php print $health['sysinfo'][0]; ?> <small><?php print $text_status; ?></small></h2>
            <h3><strong>uptime:</strong> <?php print $health['uptime']; ?></h3>
          </div>
        </div>
      </div>


      <div class="container text-start">
        <div class="row">
          <div class="col-2">Piler:</div>
          <div class="col"><?php print $health['piler_version']; ?></div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_server_operating_system; ?></div>
          <div class="col"><?php print $health['sysinfo'][1]; ?></div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_cpu_usage; ?></div>
          <div class="col">
            <div class="progress" role="progressbar" aria-label="Success example" aria-valuenow="<?php print floor($health['cpuinfo']); ?>" aria-valuemin="0" aria-valuemax="100">
              <div class="progress-bar <?php if($health['cpuinfo'] < HEALTH_RATIO) { ?>bg-success<?php } else { ?>bg-danger<?php } ?>" style="width: <?php print floor($health['cpuinfo']); ?>%"><?php print $health['cpuinfo']; ?>% <?php print $health['cpuload']; ?></div>
            </div>
          </div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_memory_usage; ?></div>
          <div class="col">
            <div class="progress" role="progressbar" aria-label="Success example" aria-valuenow="<?php print floor($health['meminfo']); ?>" aria-valuemin="0" aria-valuemax="100">
              <div class="progress-bar <?php if($health['meminfo'] < HEALTH_RATIO) { ?>bg-success<?php } else { ?>bg-danger<?php } ?>" style="width: <?php print floor($health['meminfo']); ?>%"><?php print $health['meminfo']; ?>% / <?php print $health['totalmem']; ?> MB</div>
            </div>
          </div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_swap_usage; ?></div>
          <div class="col">
            <div class="progress" role="progressbar" aria-label="Success example" aria-valuenow="<?php print floor($health['meminfo']); ?>" aria-valuemin="0" aria-valuemax="100">
              <div class="progress-bar <?php if($health['swapinfo'] < HEALTH_RATIO) { ?>bg-success<?php } else { ?>bg-danger<?php } ?>" style="width: <?php print floor($health['swapinfo']); ?>%"><?php print $health['swapinfo']; ?>% / <?php print $health['totalswap']; ?> MB</div>
            </div>
          </div>
        </div>
      </div>

<?php if(ENABLE_SAAS == 1) { ?>
      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_online_users; ?></div>
          <div class="col"><a href="<?php print PATH_PREFIX; ?>index.php?route=stat/online"><?php print $health['num_of_online_users']; ?></a></div>
        </div>
      </div>
<?php } ?>

      <div class="container text-start">
        <div class="row">
          <div class="col">
            <h2><i class="bi bi-hdd"></i> <?php print $text_storage; ?></h2>
          </div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_archive_size; ?></div>
          <div class="col"><?php print $health['archive_size']; ?>B (<?php print $health['archive_stored_size']; ?>B)</div>
        </div>
      </div>
      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_disk_usage; ?></div>
          <div class="col">
          <?php foreach($health['shortdiskinfo'] as $partition) { ?>
            <div class="<?php if($partition['utilization'] < HEALTH_RATIO) { ?>text-success<?php } else { ?>text-error<?php } ?>">
              <?php print $partition['partition']; ?>
              <?php print nice_size(1000*$partition['used']); ?> / <?php print nice_size(1000*$partition['total']); ?>
              (<?php print $partition['utilization']; ?>%)
            </div>
          <?php } ?>
          </div>
        </div>
      </div>
      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_smtp_status; ?></div>
          <div class="col">
          <?php foreach($health['health'] as $h) {
            if(preg_match("/^220/", $h[1])) {
              $status = 'OK'; $class = 'text-success';
            } else {
              $status = 'ERROR'; $class = 'text-error';
          } ?>
            <div class="<?php print $class; ?>">
              <span onmouseover="Tip('<?php print preg_replace("/\'/", "\'", $h[1]); ?>, <?php print $h[2]; ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip()"><?php print $h[3]; ?>: <?php print $status; ?></span></div>
          <?php } ?>
          </div>
        </div>
      </div>
      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_periodic_purge; ?></div>
          <div class="col">
            <span class="<?php if($health['options']['enable_purge'] == 1) { ?>ok<?php } else { ?>error<?php } ?>"><?php if($health['options']['enable_purge'] == 1) { print $text_enabled; ?>.
              <a href="<?php print HEALTH_URL; ?>&toggle_enable_purge"><?php print $text_disable; ?></a>
              <?php if($health['purge_stat'][0]) { print $text_last; ?>: <?php print $health['purge_stat'][0]; ?>, <?php print $text_next; ?>:  <?php print $health['purge_stat'][1]; } ?> <?php } else { print $text_disabled; ?>.
              <a href="<?php print HEALTH_URL; ?>&toggle_enable_purge"><?php print $text_enable; ?></a><?php } ?>
            </span>
          </div>
        </div>
      </div>

    <?php if($health['indexer_stat'][0]) { ?>
      <div class="container text-start">
        <div class="row">
          <div class="col-2"><?php print $text_indexer_job; ?></div>
          <div class="col"><?php print $text_last; ?>: <?php print $health['indexer_stat'][0]; ?>, <?php print $text_next; ?>:  <?php print $health['indexer_stat'][1]; ?></div>
        </div>
      </div>
    <?php } ?>

    <?php if(MEMCACHED_ENABLED) { ?>
      <div class="container text-start">
        <div class="row">
          <div class="col">
            <h2><i class="bi bi-database"></i> Memcached</h2>
          </div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2">Extension</div>
          <div class="col"><?php if(extension_loaded('memcached')) { ?><i class="verified bi bi-check-circle-fill text-success"></i><?php } else { ?><i class="unverified bi bi-x-square-fill text-danger"></i><?php } ?></div>
        </div>
      </div>

      <div class="container text-start">
        <div class="row">
          <div class="col-2">Version</div>
          <div class="col"><?php print $health['memcached_version']; ?></div>
        </div>
      </div>

    <?php } ?>

    </div>

    <div class="col-4"> <!-- right side -->

      <table class="table table-striped">
        <tr>
          <th colspan="2"><?php print $text_cumulative_counts; ?></th>
        </tr>
        <tr>
          <td><?php print $text_oldest_record; ?></td>
          <td><?php print date(DATE_TEMPLATE, $health['oldestmessagets']); ?></td>
        </tr>
        <tr>
          <td><?php print $text_processed_emails; ?></td>
          <td>
            <?php print format_number($health['processed_emails']['last_60_mins_count']); ?> (<?php print $text_60_minutes; ?>)<br />
            <?php print format_number($health['processed_emails']['today_count']); ?> (<?php print $text_24_hours; ?>)<br />
            <?php print format_number($health['processed_emails']['last_7_days_count']); ?> (<?php print $text_1_week; ?>)<br />
            <?php print format_number($health['processed_emails']['last_30_days_count']); ?> (<?php print $text_30_days; ?>)
          </td>
        </tr>
        <tr>
          <th colspan="2"><?php print $text_message_disposition; ?></th>
        </tr>
      <?php foreach($health['counters'] as $k => $v) { if(!is_numeric($k)) { ?>
        <tr>
          <td><?php $a = preg_replace("/^_piler\:/", "", $k); if(isset($$a)) { print $$a; } else { print $k; } ?></td>
          <td><?php print format_number($v); ?></td>
        </tr>
      <?php } } ?>
      </table>

      <h4><?php print $text_space_projection; ?></h4>
      <table class="table table-striped">
        <tr>
          <td><?php print $text_average_messages_day; ?></td>
          <td><?php print $health['averagemessages']; ?></td>
        </tr>
        <tr>
          <td><?php print $text_average_message_size; ?></td>
          <td><?php print $health['averagemessagesize']; ?> + <?php print $health['averagesqlsize']; ?> + <?php print $health['averagesphinxsize']; ?></td>
        </tr>
        <tr>
          <td><?php print $text_average_size_day; ?></td>
          <td><?php print $health['averagesizeday']; ?></td>
        </tr>
        <tr>
          <td>"<?php print DATA_PARTITION; ?>" <?php print $text_partition_full; ?></td>
          <td><?php print $health['daysleftatcurrentrate'][0]; ?> years, <?php print $health['daysleftatcurrentrate'][1]; ?> months, <?php print $health['daysleftatcurrentrate'][2]; ?> days</td>
        </tr>
        <tr>
          <td><?php print $text_usage_trend; ?></td>
          <td><?php if ($health['usagetrend'] > 0) { print $text_usage_increasing; } elseif($health['usagetrend'] < 0) { print $text_usage_decreasing; } else { print $text_usage_neutral; } ?></td>
        </tr>
        <tr>
          <td>Sphinx total index</td>
          <td><?php print nice_size($health['sphinx_total_size']); ?></td>
        </tr>
      </table>

    </div>
  </div>
</div>
