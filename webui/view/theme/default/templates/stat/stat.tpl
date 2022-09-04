<p>

<?php if($timespan == "daily"){ $date_format = "H:i"; ?>
<strong><?php print $text_daily_report; ?></strong> <a href="index.php?route=stat/stat&amp;timespan=monthly<?php if(isset($uid)) { ?>&amp;uid=<?php print $uid; } ?>"><?php print $text_monthly_report; ?></a>
<?php } else { $date_format = "m.d."; ?>
<a href="index.php?route=stat/stat&amp;timespan=daily<?php if(isset($uid)) { ?>&amp;uid=<?php print $uid; } ?>"><?php print $text_daily_report; ?></a> <strong><?php print $text_monthly_report; ?></strong>
<?php } ?>

</p>


<div>
  <canvas id="myChart"></canvas>
</div>

<!-- See the docs at https://www.chartjs.org/docs/latest/ -->
<script src="https://cdn.jsdelivr.net/npm/chart.js"></script>

<script>
  const labels = [
<?php foreach(array_keys($data) as $d) { print "'" . date($date_format, $d) . "',"; } ?>
  ];

  const data = {
    labels: labels,
    datasets: [{
      label: '<?php print $text_archived_messages; ?>',
      backgroundColor: 'rgb(255, 99, 132)',
      borderColor: 'rgb(255, 99, 132)',
      data: [
<?php foreach(array_values($data) as $d) { print "'" . $d . "',"; } ?>
      ],
    }]
  };

  const config = {
    type: 'line',
    data: data,
    options: {}
  };

  const chart = new Chart(document.getElementById('myChart'), config);
</script>
