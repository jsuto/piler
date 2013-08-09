<?php if(Registry::get('username')) { ?>

    <div class="navbar navbar-fixed-top">
      <div class="navbar-inner">
        <div class="container-fluid"<?php if($settings['background_colour']) { ?> style="background: <?php print $settings['background_colour']; ?>;"<?php } ?>>
        
          <!-- <a class="btn btn-navbar" data-toggle="collapse" data-target=".nav-collapse">
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </a> -->
        
          <a class="brand" target="_blank" href="<?php print $settings['branding_url']; ?>" title="<?php print $settings['branding_text']; ?>"><?php if($settings['branding_logo']) { ?><img src="/images/<?php print $settings['branding_logo']; ?>" alt="<?php print $settings['branding_text']; ?>" /><?php } ?></a>
          
          
          <!-- <div class="nav-collapse"> -->
            <ul class="nav">

<?php if($admin_user == 1) { ?>

                                <li class="dropdown">
                                        <a href="#" class="dropdown-toggle" data-toggle="dropdown" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-desktop"></i>&nbsp;<?php print $text_monitor; ?>&nbsp;<b class="caret"></b></a>
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
                                   <a href="#" class="dropdown-toggle" data-toggle="dropdown" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-cogs"></i>&nbsp;<?php print $text_administration; ?>&nbsp;<b class="caret"></b></a>
                                   <ul class="dropdown-menu">
                                          <li><a href="index.php?route=user/list"><i class="icon-user"></i>&nbsp;<?php print $text_users; ?></a></li>
                                          <li><a href="index.php?route=group/list"><i class="icon-group"></i>&nbsp;<?php print $text_groups; ?></a></li>
                                          <li><a href="index.php?route=domain/domain"><i class="icon-globe"></i>&nbsp;<?php print $text_domain; ?></a></li>
                                       <?php if(ENABLE_SAAS == 1) { ?>
                                          <li><a href="index.php?route=ldap/list"><i class="icon-key"></i>&nbsp;<?php print $text_ldap; ?></a></li>
                                          <li><a href="index.php?route=customer/list"><i class="icon-wrench"></i>&nbsp;<?php print $text_customers; ?></a></li>
                                       <?php } ?>
                                          <li><a href="index.php?route=policy/archiving"><i class="icon-folder-open"></i>&nbsp;<?php print $text_archiving_rules; ?></a></li>
                                          <li><a href="index.php?route=policy/retention"><i class="icon-time"></i>&nbsp;<?php print $text_retention_rules; ?></a></li>
                                   </ul>
                                </li>


<?php } else { ?>

              <li><a href="search.php" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-search"></i>&nbsp;<?php print $text_search; ?></a></li>

          <?php if(ENABLE_AUDIT == 1 && $auditor_user == 1) { ?>
              <li><a href="index.php?route=audit/audit" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-book"></i>&nbsp;<?php print $text_audit; ?></a></li>
          <?php } ?>

          <?php if($settings['support_link']) { ?>
              <li><a href="<?php print $settings['support_link']; ?>" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><?php print $text_contact_support; ?></a></li>
          <?php } ?>

	  <?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
              <li><a href="/folders.php" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-folder-close"></i>&nbsp;<?php print $text_folders; ?></a></li>
          <?php } ?>

<?php } ?>


            </ul>
            
            <ul class="nav pull-right">
		<?php if($settings['branding_url']) { ?><li><a href="<?php print $settings['branding_url']; ?>" target="_blank" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-phone"></i>&nbsp;<?php print $settings['branding_text']; ?></a></li><?php } ?>

				<li class="dropdown">
				<a href="#" class="dropdown-toggle" data-toggle="dropdown" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-user"></i>&nbsp;<?php if(isset($_SESSION['realname'])) { print $_SESSION['realname']; ?>&nbsp;<?php } ?><b class="caret"></b></a>
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
