<div class="container">
	<div class="alert alert-info"><strong><?php print $text_refresh_period; ?>:</strong> <?php print HEALTH_REFRESH; ?> sec</div>

	<div class="row">
		
		<div class="span8">
            <h2><i class="icon-dashboard icon-2x pull-left"></i><?php print $health['sysinfo'][0]; ?> <small><?php print $text_status; ?></small></h2>
            <h3><strong>Up For:</strong> <?php print $health['uptime']; ?></h3>
            <table class="table table-striped">
                <tr>
                    <th class="span4">Piler:</th>
                    <td class="span8"> <?php print $health['piler_version']; ?></td>
                </tr>

                <tr>
                    <th class="span4"><?php print $text_server_operating_system; ?>:</th>
                    <td class="span8"> <?php print $health['sysinfo'][1]; ?></td>
             	</tr>        
            
            	<tr>
					<th><?php print $text_cpu_usage; ?>:</th>
					<td><div class="progress <?php if($health['cpuinfo'] < HEALTH_RATIO) { ?>progress-success<?php } else { ?>progress-danger<?php } ?>"><div class="bar" style="width: <?php print $health['cpuinfo']; ?>%"></div>&nbsp;<?php print $health['cpuinfo']; ?>% <?php print $health['cpuload']; ?></div></td>
                </tr>

                <tr>
					<th><?php print $text_memory_usage; ?>:</th>
                    <td><div class="progress <?php if($health['meminfo'] < HEALTH_RATIO) { ?>progress-success<?php } else { ?>progress-danger<?php } ?>"><div class="bar" style="width: <?php print $health['meminfo']; ?>%"></div>&nbsp;<?php print $health['meminfo']; ?>% / <?php print $health['totalmem']; ?> MB</div></td>
				</tr>
                
                <tr>
					<th><?php print $text_swap_usage; ?>:</th>
                    <td><div class="progress <?php if($health['swapinfo'] < HEALTH_RATIO) { ?>progress-success<?php } else { ?>progress-danger<?php } ?>"><div class="bar" style="width: <?php print $health['swapinfo']; ?>%"></div>&nbsp;<?php print $health['swapinfo']; ?>% / <?php print $health['totalswap']; ?> MB</div></td>
				</tr>

<?php if(ENABLE_SAAS == 1) { ?>
                <tr>
                    <th class="span4"><?php print $text_online_users; ?>:</th>
                    <td class="span8"><a href="index.php?route=stat/online"><?php print $health['num_of_online_users']; ?></a></td>
                </tr>
<?php } ?>
                
            </table>
            
            <h2><i class="icon-hdd icon-2x pull-left"></i>&nbsp;<?php print $text_storage; ?></h2>
			<table class="table table-striped">
				 <tr>
					<th><?php print $text_archive_size; ?>:</th>
					<td><?php print $health['archive_size']; ?>B (<?php print $health['archive_stored_size']; ?>B)</td>
				 </tr>

				 <tr>
					<th><?php print $text_disk_usage; ?>:</th>
                                        <td>
                                            <table>
                                                <?php foreach($health['shortdiskinfo'] as $partition) { ?>

                                                   <tr class="<?php if($partition['utilization'] < HEALTH_RATIO) { ?>text-success<?php } else { ?>text-error<?php } ?>">
                                                      <td><?php print $partition['partition']; ?></td>
                                                      <td><?php print nice_size(1000*$partition['used']); ?> / <?php print nice_size(1000*$partition['total']); ?></td>
                                                      <td>(<?php print $partition['utilization']; ?>%)</td>
                                                   </tr>

                                                <?php } ?>

                                            </table>
                                        </td>
				 </tr>
		
				 <tr>
					<th><?php print $text_smtp_status; ?>:</th>
					<td>
					   <?php foreach($health['health'] as $h) {
								if(preg_match("/^220/", $h[1])) {
								   $status = 'OK'; $class = 'text-success';
								} else {
								   $status = 'ERROR'; $class = 'text-error';
								}
					   ?>
								<div class="<?php print $class; ?>"><span onmouseover="Tip('<?php print preg_replace("/\'/", "\'", $h[1]); ?>, <?php print $h[2]; ?>', BALLOON, true, ABOVE, true)" onmouseout="UnTip()"><?php print $h[3]; ?>: <?php print $status; ?></span></div>
					   <?php } ?>
					</td>
				 </tr>

				 <tr>
					<th><?php print $text_periodic_purge; ?></th>
					<td>
                                           <span class="<?php if($health['options']['enable_purge'] == 1) { ?>ok<?php } else { ?>error<?php } ?>"><?php if($health['options']['enable_purge'] == 1) { print $text_enabled; ?>. <a href="<?php print HEALTH_URL; ?>&toggle_enable_purge"><?php print $text_disable; ?></a>
                                              <?php if($health['purge_stat'][0]) { print $text_last; ?>: <?php print $health['purge_stat'][0]; ?>, <?php print $text_next; ?>:  <?php print $health['purge_stat'][1]; } ?>
                                           <?php } else { print $text_disabled; ?>. <a href="<?php print HEALTH_URL; ?>&toggle_enable_purge"><?php print $text_enable; ?></a><?php } ?>
                                           </span>
                                        </td>
				 </tr>

                         <?php if($health['indexer_stat'][0]) { ?>
                                 <tr>
                                        <th><?php print $text_indexer_job; ?></th>
                                        <td><?php print $text_last; ?>: <?php print $health['indexer_stat'][0]; ?>, <?php print $text_next; ?>:  <?php print $health['indexer_stat'][1]; ?></td>

                                 </tr>
                         <?php } ?>

			 </table>
		</div>
				 
		 <div class="span4">		
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
                                           <?php print $health['processed_emails']['last_60_mins_count']; ?> (<?php print $text_60_minutes; ?>)<br />
                                           <?php print $health['processed_emails']['today_count']; ?> (<?php print $text_24_hours; ?>)<br />
                                           <?php print $health['processed_emails']['last_7_days_count']; ?> (<?php print $text_1_week; ?>)<br />
                                           <?php print $health['processed_emails']['last_30_days_count']; ?> (<?php print $text_30_days; ?>)
                                        </td>
				</tr>
				<tr>
					<th colspan="2"><?php print $text_message_disposition; ?></th>
				</tr>
			   <?php while(list($k, $v) = each($health['counters'])) {
						if(!is_numeric($k)) { ?>
						   <tr>
							  <td><?php $a = preg_replace("/^_piler\:/", "", $k); if(isset($$a)) { print $$a; } else { print $k; } ?></td>
							  <td><?php print $v; ?></td>
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
                                  <td>Sphinx main index</td>
                                  <td<?php if($health['sphinx_current_main_size'] > SPHINX_MAIN_INDEX_THRESHOLD) { ?> class="text-error"<?php } ?>><?php print nice_size($health['sphinx_current_main_size']); ?></td>
                           </tr>
		 </table>
          
          
		 </div>
	</div>
</div>


</div>


