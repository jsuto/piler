   <div id="search">

      <div id="health1">

         <div class="row">
            <div class="cellhealthleft"><?php print $text_refresh_period; ?>:</div>
            <div class="cellhealthright"><?php print HEALTH_REFRESH; ?> sec</div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_server_name; ?>:</div>
            <div class="cellhealthright"><?php print $sysinfo[0]; ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_server_operating_system; ?>:</div>
            <div class="cellhealthright"><?php print $sysinfo[1]; ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_uptime; ?>:</div>
            <div class="cellhealthright"><?php print $uptime; ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_archive_size; ?>:</div>
            <div class="cellhealthright"><?php print $archive_size; ?>B</div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_smtp_status; ?>:</div>
            <div class="cellhealthright">
               <?php foreach($health as $h) {
                        if(preg_match("/^220/", $h[1])) {
                           $status = 'OK'; $class = 'ok';
                        } else {
                           $status = 'ERROR'; $class = 'error';
                        }
               ?>
                        <div class="<?php print $class; ?>"><span><?php print $h[3]; ?>: <?php print $status; ?></span></div>
               <?php } ?>
            </div>
         </div>


         <div class="row">
            <div class="cellhealthleft"><?php print $text_processed_emails; ?>:</div>
            <div class="cellhealthright"><?php print $processed_emails[0]; ?> (<?php print $text_24_hours; ?>)<br /><?php print $processed_emails[1]; ?> (<?php print $text_1_week; ?>)<br /><?php print $processed_emails[2]; ?> (<?php print $text_30_days; ?>)</div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_cpu_usage; ?>:</div>
            <div class="cellhealthright"><span class="<?php if($cpuinfo < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $cpuinfo; ?>% <?php print $cpuload; ?></span></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_memory_usage; ?>:</div>
            <div class="cellhealthright"><span class="<?php if($meminfo < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $meminfo; ?>% / <?php print $totalmem; ?> MB</span></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_swap_usage; ?></div>
            <div class="cellhealthright"><span class="<?php if($swapinfo < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $swapinfo; ?>% / <?php print $totalswap; ?> MB</span></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_disk_usage; ?></div>
            <div class="cellhealthright"><?php foreach($shortdiskinfo as $partition) { ?><span class="<?php if($partition['utilization'] < HEALTH_RATIO) { ?>ok<?php } else { ?>error<?php } ?>"><?php print $partition['partition']; ?> <?php print $partition['utilization']; ?>%</span> <?php } ?></div>
         </div>

         <div class="row">
            <div class="cellhealthleft"><?php print $text_periodic_purge; ?></div>
            <div class="cellhealthright"><span class="<?php if($options['enable_purge'] == 1) { ?>ok<?php } else { ?>error<?php } ?>"><?php if($options['enable_purge'] == 1) { print $text_enabled; ?>. <a href="<?php print HEALTH_URL; ?>&toggle_enable_purge"><?php print $text_disable; ?></a><?php } else { print $text_disabled; ?>. <a href="<?php print HEALTH_URL; ?>&toggle_enable_purge"><?php print $text_enable; ?></a><?php } ?> </span></div>
         </div>


         <div class="row">
            <div class="cellhealthleft"><?php print $text_counters; ?></div>
            <div class="cellhealthright">
               <?php while(list($k, $v) = each($counters)) {
                        if(!is_numeric($k)) { ?>
                           <div class="row">
                              <div class="cellhealthright"><?php $a = preg_replace("/^_piler\:/", "", $k); if(isset($$a)) { print $$a; } else { print $k; } ?></div>
                              <div class="cellhealthright"><?php print $v; ?></div>
                           </div>
                        <?php } } ?>
            </div>
         </div>

      </div>


      <div id="health2">
<?php if(isset($queues)) { ?>
         <div class="row">
            <div class="cellhealthleft"><?php print $text_queue_status; ?></div>
         </div>

         <?php foreach ($queues as $queue) {

                  if(isset($queue['desc'])) { ?>

               <div class="row">
                  <div class="cellhealthleft"><?php print $queue['desc']; ?></div>
               </div>

               <div class="row">
                  <div class="cellhealthright"><pre><?php print $queue['lines']; ?></pre></div>
               </div>

            <?php } 
           } ?>

<?php } ?>
      </div>

   </div>


