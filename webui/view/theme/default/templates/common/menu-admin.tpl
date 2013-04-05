<?php if(Registry::get('username')) { ?>

    <div class="navbar navbar-fixed-top">
      <div class="navbar-inner">
        <div class="container">
        
          <!-- <a class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </a> -->
          
          <a class="brand" href="/index.php?route=health/health" title="<?php print SITE_NAME; ?>"><img src="<?php print SITE_LOGO_SM; ?>" alt="<?php print SITE_NAME; ?>" /></a>
          
          <!-- <div class="nav-collapse"> -->
          
            <ul class="nav">
				<li class="dropdown">
					<a href="#" class="dropdown-toggle" data-toggle="dropdown"><i class="icon-desktop"></i>&nbsp;<?php print $text_monitor; ?>&nbsp;<b class="caret"></b></a>
					<ul class="dropdown-menu">
						<li><a href="index.php?route=stat/stat&timespan=daily"><i class="icon-bar-chart"></i>&nbsp;Statistics</a></li>
                        <li><a href="index.php?route=accounting/accounting&view=email"><i class="icon-tasks"></i>&nbsp;Accounting</a></li>
						<li><a href="index.php?route=health/health"><i class="icon-medkit"></i>&nbsp;Health</a></li>
						<?php if(ENABLE_AUDIT == 1) { ?>
						<li><a href="index.php?route=audit/audit"><i class="icon-book"></i>&nbsp;<?php print $text_audit; ?></a></li>
						<?php } ?>
					</ul>
				</li>
				<li class="dropdown"<?php if(strstr($_SERVER['QUERY_STRING'], "domain/") || ($_SERVER['QUERY_STRING'] != "route=user/settings" && strstr($_SERVER['QUERY_STRING'], "user/")) || strstr($_SERVER['QUERY_STRING'], "policy/") || strstr($_SERVER['QUERY_STRING'], "import/")) { ?> id="active"<?php } ?>>
				   <a href="#" class="dropdown-toggle" data-toggle="dropdown"><i class="icon-cogs"></i>&nbsp;<?php print $text_administration; ?>&nbsp;<b class="caret"></b></a>
				   <ul class="dropdown-menu">
					  <li><a href="index.php?route=user/list"><i class="icon-user"></i>&nbsp;<?php print $text_users; ?></a></li>
					  <li><a href="index.php?route=group/list"><i class="icon-group"></i>&nbsp;<?php print $text_groups; ?></a></li>
					  <li><a href="index.php?route=domain/domain"><i class="icon-globe"></i>&nbsp;<?php print $text_domain; ?></a></li>
					  <li><a href="index.php?route=policy/archiving"><i class="icon-folder-open"></i>&nbsp;<?php print $text_archiving_rules; ?></a></li>
					  <li><a href="index.php?route=policy/retention"><i class="icon-time"></i>&nbsp;<?php print $text_retention_rules; ?></a></li>
				   </ul>
				</li>
			</ul>
            <ul class="nav pull-right">
                <li class="dropdown">
                    <a href="#" class="dropdown-toggle" data-toggle="dropdown"><i class="icon-user icon-white"></i>&nbsp;<?php if(isset($_SESSION['realname'])) { print $_SESSION['realname']; ?>&nbsp;<?php } ?><b class="caret"></b></a>
                    <ul class="dropdown-menu">
                        <li><a href="settings.php"><i class="icon-cog"></i>&nbsp;Settings</a></li>
                        <li class="divider"></li>
                        <li><a href="logout.php"><i class="icon-off"></i>&nbsp;<?php print $text_logout; ?></a></li>
                    </ul>
                </li>
            </ul>
              
          <!-- </div><!--/.nav-collapse -->
        </div>
      </div>
    </div>
<?php } ?>
