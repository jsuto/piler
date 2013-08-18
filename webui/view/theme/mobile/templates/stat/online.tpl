
<h4><?php print $text_online_users; ?>, <?php print $text_refresh_period; ?>: <?php print HEALTH_REFRESH; ?> sec</h4>

<div class="listarea">

<?php if(isset($users)){ ?>

   <div id="ss1">
      <div class="domainrow">
         <div class="domaincell"><?php print $text_username; ?></div>
         <div class="domaincell"><?php print $text_ipaddr; ?></div>
         <div class="domaincell"><?php print $text_logged_in; ?></div>
         <div class="domaincell"><?php print $text_last_activity; ?></div>
      </div>

<?php foreach($users as $user) { ?>
      <div class="domainrow">
         <div class="domaincell"><?php print $user['username']; ?></div>
         <div class="domaincell"><?php if(DEMO_MODE == 1) { print anonimize_ip_addr($user['ipaddr']); } else { print $user['ipaddr']; } ?></div>
         <div class="domaincell"><?php print date(DATE_TEMPLATE . " H:i:s", $user['ts']); ?></div>
         <div class="domaincell"><?php print date(DATE_TEMPLATE . " H:i:s", $user['last_activity']); ?></div>
      </div>
<?php } ?>

   </div>

<?php } ?>

</div>

