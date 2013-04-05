<?php if(Registry::get('username')) { ?>

    <div class="navbar">
       <div class="navbar-inner">

       <?php if(BRANDING_TEXT) { ?>
          <a class="brand" href="<?php if(BRANDING_URL) { print BRANDING_URL; } else { ?>#<?php } ?>"><?php print BRANDING_TEXT; ?></a>
       <?php } ?>

          <ul class="nav pull-left">

<?php if($admin_user == 1) { ?>

             <li class="dropdown<?php if(strstr($_SERVER['QUERY_STRING'], "stat/") || strstr($_SERVER['QUERY_STRING'], "health/") || strstr($_SERVER['QUERY_STRING'], "audit/") ) { ?> active<?php } ?>">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown"><?php print $text_monitor; ?> <b class="caret"></b></a>
                <ul class="dropdown-menu">
                   <li><a href="index.php?route=stat/stat&timespan=daily"><?php print $text_statistics; ?></a></li>
                   <li><a href="index.php?route=health/health"><?php print $text_health; ?></a></li>
                   <li><a href="index.php?route=accounting/accounting&view=email"><?php print $text_accounting; ?></a></li>
               <?php if(ENABLE_AUDIT == 1) { ?>
                   <li><a href="index.php?route=audit/audit"><?php print $text_audit; ?></a></li>
               <?php } ?>
                </ul>
             </li>

             <li class="divider-vertical"></li>

             <li class="dropdown<?php if(strstr($_SERVER['QUERY_STRING'], "domain/") || $_SERVER['QUERY_STRING'] == "route=user/list" || strstr($_SERVER['QUERY_STRING'], "group/") || strstr($_SERVER['QUERY_STRING'], "policy/") ) { ?> active<?php } ?>">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown"><?php print $text_administration; ?> <b class="caret"></b></a>
                 <ul class="dropdown-menu">
                    <li><a href="index.php?route=user/list"><?php print $text_users; ?></a></li>
                    <li><a href="index.php?route=group/list"><?php print $text_groups; ?></a></li>
                    <li><a href="index.php?route=domain/domain"><?php print $text_domain; ?></a></li>
                    <li><a href="index.php?route=policy/archiving"><?php print $text_archiving_rules; ?></a></li>
                    <li><a href="index.php?route=policy/retention"><?php print $text_retention_rules; ?></a></li>
                 </ul>
             </li>

<?php } else { ?>

             <li<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> class="active"<?php } ?>><a href="search.php"><?php print $text_search; ?></a></li>
          <?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
             <li<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "folders.php")){ ?> class="active"<?php } ?>><a href="folders.php"><?php print $text_folder; ?></a></li>
          <?php } ?>

<?php } ?>

          </ul>

          <ul class="nav pull-right">
             <li class="divider-vertical"></li>
             <li class="dropdown">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown"><?php print $_SESSION['realname']; ?> <i class="icon-user"></i> <b class="caret"></b></a>
                <ul class="dropdown-menu">
                   <li><a href="settings.php"><?php print $text_settings; ?></a></li>
                   <li class="divider"></li>
                   <li><a href="logout.php"><?php print $text_logout; ?></a></li>
                </ul>
             </li>
          </ul>

       </div>
    </div>

<?php } ?>

