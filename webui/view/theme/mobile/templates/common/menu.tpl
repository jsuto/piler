<?php if(Registry::get('username')) { ?>

    <div class="navbar navbar-inverse">
       <div class="navbar-inner"<?php if($settings['background_colour']) { ?> style="background: <?php print $settings['background_colour']; ?>;"<?php } ?>>


       <a class="brand" target="_blank" href="<?php print $settings['branding_url']; ?>" title="<?php print $settings['branding_text']; ?>"><?php if($settings['branding_logo']) { ?><img src="/images/<?php print $settings['branding_logo']; ?>" alt="<?php print $settings['branding_text']; ?>" /><?php } ?></a>

          <ul class="nav pull-left">

<?php if($admin_user == 1) { ?>

             <li class="dropdown<?php if(strstr($_SERVER['QUERY_STRING'], "stat/") || strstr($_SERVER['QUERY_STRING'], "health/") || strstr($_SERVER['QUERY_STRING'], "audit/") ) { ?> active<?php } ?>">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-eye-open icon-white"></i>&nbsp;<?php print $text_monitor; ?> <b class="caret"></b></a>
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
                <a href="#" class="dropdown-toggle" data-toggle="dropdown" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-wrench icon-white"></i>&nbsp;<?php print $text_administration; ?> <b class="caret"></b></a>
                 <ul class="dropdown-menu">
                    <li><a href="index.php?route=user/list"><?php print $text_users; ?></a></li>
                    <li><a href="index.php?route=group/list"><?php print $text_groups; ?></a></li>
                    <li><a href="index.php?route=domain/domain"><?php print $text_domain; ?></a></li>
     <?php if(ENABLE_SAAS == 1) { ?>
                    <li><a href="index.php?route=ldap/list"><?php print $text_ldap; ?></a></li>
                    <li><a href="index.php?route=customer/list"><?php print $text_customers; ?></a></li>
                    <li><a href="index.php?route=import/list"><?php print $text_import; ?></a></li>
     <?php } ?>
                    <li><a href="index.php?route=policy/archiving"><?php print $text_archiving_rules; ?></a></li>
                    <li><a href="index.php?route=policy/retention"><?php print $text_retention_rules; ?></a></li>
                    <li><a href="index.php?route=search/autosearch"><?php print $text_automated_search; ?></a></li>
                 </ul>
             </li>

     <?php if(LDAP_ADMIN_MEMBER_DN) { ?>
             <li<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> class="active"<?php } ?>><a href="search.php" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-search icon-white"></i>&nbsp;<?php print $text_search; ?></a></li>
     <?php } ?>

<?php } else { ?>

             <li<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> class="active"<?php } ?>><a href="search.php" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-search icon-white"></i>&nbsp;<?php print $text_search; ?></a></li>

          <?php if(ENABLE_FOLDER_RESTRICTIONS == 1) { ?>
             <li<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "folders.php")){ ?> class="active"<?php } ?>><a href="folders.php" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><?php print $text_folder; ?></a></li>
          <?php } ?>

          <?php if(ENABLE_AUDIT == 1 && $auditor_user == 1) { ?>
              <li<?php if(strstr($_SERVER['REQUEST_URI'], "audit/audit")){ ?> class="active"<?php } ?>><a href="index.php?route=audit/audit" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-book icon-white"></i>&nbsp;<?php print $text_audit; ?></a></li>
          <?php } ?>

          <?php if($settings['support_link']) { ?>
              <li><a href="<?php print $settings['support_link']; ?>" target="_blank" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><?php print $text_contact_support; ?></a></li>
          <?php } ?>

<?php } ?>

          </ul>

          <ul class="nav pull-right">

             <?php if($settings['branding_url']) { ?><li><a href="<?php print $settings['branding_url']; ?>" target="_blank" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><i class="icon-share-alt icon-white"></i> <?php print $settings['branding_text']; ?></a></li><?php } ?>

             <li class="dropdown">
                <a href="#" class="dropdown-toggle" data-toggle="dropdown" <?php if($settings['text_colour']) { ?> style="color: <?php print $settings['text_colour']; ?>;"<?php } ?>><?php print $realname; ?> <i class="icon-user icon-white"></i> <b class="caret"></b></a>
                <ul class="dropdown-menu">
                   <li><a href="settings.php"><i class="icon-cog"></i>&nbsp;<?php print $text_settings; ?></a></li>
                   <li class="divider"></li>
                   <li><a href="logout.php"><i class="icon-off"></i>&nbsp;<?php print $text_logout; ?></a></li>
                </ul>
             </li>
          </ul>

       </div>
    </div>

<?php } ?>

