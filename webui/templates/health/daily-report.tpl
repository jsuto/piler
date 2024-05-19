<html>
<head>
   <title><?php print $text_daily_piler_report; ?>, <?php print $date; ?></title>

   <meta http-equiv="content-type" content="text/html; charset=utf-8" />
   <meta http-equiv="Content-Language" content="<?php print getenv("LANG"); ?>" />

   <style type="text/css">
        .row { display: table-row; }

        #health1 { display: table-cell; margin-top: 10px; margin-bottom: 0; }
        #health2 { display: table-cell; text-align: right; }

        .cellhealthleft { display: table-cell; height: 25px; text-align: left; padding: 3px; font: bold 12px Arial, sans-serif; }
        .cellhealthright { display: table-cell; height: 25px; text-align: left; padding: 3px; font: normal 12px Arial, sans-serif; }

        #health1 .ok { color: green; font-weight: bold; }
        #health1 .error { color: red; font-weight: bold; }

   </style>


</head>
<body>

<h1><?php print $text_daily_piler_report; ?>, <?php print $date; ?></h1>

<div id="search">

      <div id="health1">


         <div class="row">
            <div class="cellhealthleft"><?php print $text_server_name; ?>:</div>
            <div class="cellhealthright"><?php print $health['sysinfo'][0]; ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_server_operating_system; ?>:</div>
            <div class="cellhealthright"><?php print $health['sysinfo'][1]; ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_uptime; ?>:</div>
            <div class="cellhealthright"><?php print $health['uptime']; ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_archive_size_before_compression; ?>:</div>
            <div class="cellhealthright"><?php print $health['archive_size']; ?>B</div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_smtp_status; ?>:</div>
            <div class="cellhealthright">
               <?php foreach($health['health'] as $h) {
                        if(preg_match("/^220/", $h[1])) {
                           $status = 'OK'; $class = 'ok';
                        } else {
                           $status = 'ERROR'; $class = 'error';
                        }
               ?>
                        <div class="<?php print $class; ?>"><?php print $h[3]; ?>: <?php print $status; ?> (<?php print $h[1]; ?>, <?php print $h[2]; ?>)</div>
               <?php } ?>
            </div>
         </div>


         <div class="row">
            <div class="cellhealthleft"><?php print $text_processed_emails; ?>:</div>
            <div class="cellhealthright"><?php print $health['processed_emails']['last_60_mins_count']; ?> (<?php print $text_60_minutes; ?>)<br /><?php print $health['processed_emails']['today_count']; ?> (<?php print $text_24_hours; ?>)<br /><?php print $health['processed_emails']['last_7_days_count']; ?> (<?php print $text_1_week; ?>)<br /><?php print $health['processed_emails']['last_30_days_count']; ?> (<?php print $text_30_days; ?>)</div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_cpu_usage; ?>:</div>
            <div class="cellhealthright"><span class="<?php if($health['cpuinfo'] < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $health['cpuinfo']; ?>% <?php print $health['cpuload']; ?></span></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_memory_usage; ?>:</div>
            <div class="cellhealthright"><span class="<?php if($health['meminfo'] < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $health['meminfo']; ?>% / <?php print $health['totalmem']; ?> MB</span></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_swap_usage; ?></div>
            <div class="cellhealthright"><span class="<?php if($health['swapinfo'] < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $health['swapinfo']; ?>% / <?php print $health['totalswap']; ?> MB</span></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_disk_usage; ?></div>
            <div class="cellhealthright"><?php foreach($health['shortdiskinfo'] as $partition) { ?><span class="<?php if($partition['utilization'] < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $partition['partition']; ?> <?php print $partition['utilization']; ?>%</span> <?php } ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_space_projection; ?></div>
            <div class="cellhealthright">
               <?php print $health['averagemessages']; ?> (<?php print $text_average_messages_day; ?>)<br/>
               <?php print $health['averagemessagesize']; ?> + <?php print $health['averagesqlsize']; ?> + <?php print $health['averagesphinxsize']; ?> (<?php print $text_average_message_size; ?>)<br/>
               <?php print $health['averagesizeday']; ?> (<?php print $text_average_size_day; ?>)<br/>
               <?php print $health['daysleftatcurrentrate'][0]; ?> years, <?php print $health['daysleftatcurrentrate'][1]; ?> months, <?php print $health['daysleftatcurrentrate'][2]; ?> days ("<?php print DATA_PARTITION; ?>" <?php print $text_partition_full; ?>)<br/>
               <?php if ($health['usagetrend'] > 0) { print $text_usage_increasing; } elseif($health['usagetrend'] < 0) { print $text_usage_decreasing; } else { print $text_usage_neutral; } ?> (<?php print $text_usage_trend; ?>)<br/>
            </div>
         </div>

</div></div>

</body>
</html>
