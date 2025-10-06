<?php if(Registry::get('username')) { ?>

       <div id="menu" class="row w-100">
         <div class="col">
          <div class="d-flex align-items-center">

          <?php if(FULL_GUI) { ?>
           <a class="menulink"<?php if($settings['branding_url']) { ?> target="_blank" href="<?php print $settings['branding_url']; ?>" title="<?php print $settings['branding_text']; ?>"<?php } ?>><?php if($settings['branding_logo']) { ?><img class="branding_logo" src="<?php print $settings['branding_logo']; ?>" alt="<?php print $settings['branding_text']; ?>"/><?php } ?></a>
          <?php } ?>

        <?php if($admin_user) { ?>

          <a href="#" class="menulink dropdown-toggle" data-bs-toggle="dropdown" aria-expanded="false"><i class="bi bi-speedometer"></i>&nbsp;<?php print $text_monitor; ?><b class="caret"></b></a>
          <ul class="dropdown-menu">
          <?php if(!EXTERNAL_DASHBOARD_URL) { ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=stat/stat&timespan=daily"><i class="bi bi-bar-chart"></i>&nbsp;<?php print $text_statistics; ?></a></li>
          <?php } ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=accounting/accounting&view=email"><i class="bi bi-wallet"></i>&nbsp;<?php print $text_accounting; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=health/health"><i class="bi bi-heart-pulse"></i>&nbsp;<?php print $text_health; ?></a></li>
          <?php if(ENABLE_AUDIT) { ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=audit/audit"><i class="bi bi-book"></i>&nbsp;<?php print $text_audit; ?></a></li>
          <?php } ?>
          </ul>

          <a href="#" class="menulink dropdown-toggle" data-bs-toggle="dropdown" aria-expanded="false"><i class="bi bi-gear"></i>&nbsp;<?php print $text_administration; ?><b class="caret"></b></a>
          <ul class="dropdown-menu">
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=user/list"><i class="bi bi-person"></i>&nbsp;<?php print $text_users; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=group/list"><i class="bi bi-people"></i>&nbsp;<?php print $text_groups; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=domain/domain"><i class="bi bi-globe"></i>&nbsp;<?php print $text_domain; ?></a></li>
          <?php if(ENABLE_SAAS == 1) { ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=ldap/list"><i class="bi bi-key"></i>&nbsp;<?php print $text_ldap; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=customer/list"><i class="bi bi-wrench"></i>&nbsp;<?php print $text_customers; ?></a></li>
          <?php } ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=import/list"><i class="bi bi-lightbulb"></i>&nbsp;<?php print $text_import; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=policy/exclusion"><i class="bi bi-shield-exclamation"></i>&nbsp;<?php print $text_exclusion_rules; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=policy/retention"><i class="bi bi-hourglass"></i>&nbsp;<?php print $text_retention_rules; ?></a></li>
         <?php if(ENABLE_FOLDER_RESTRICTIONS) { ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=policy/folder"><i class="bi bi-folder2-open"></i>&nbsp;<?php print $text_folder_rules; ?></a></li>
         <?php } ?>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=policy/legalhold"><i class="bi bi-hammer"></i>&nbsp;<?php print $text_legal_hold; ?></a></li>
            <li class="dropdown-item"><a class="dropdown-item" href="<?php print PATH_PREFIX; ?>index.php?route=search/autosearch"><i class="bi bi-search"></i>&nbsp;<?php print $text_automated_search; ?></a></li>
          </ul>
        <?php } ?>

        <?php if(!$admin_user) { ?>
           <a href="<?php print PATH_PREFIX; ?>search.php" class="menulink"><i class="bi bi-search"></i><?php if(FULL_GUI) { print $text_search; } ?></a>
        <?php } ?>

        <?php if($auditor_user && ENABLE_AUDIT) { ?>
           <a href="<?php print PATH_PREFIX; ?>index.php?route=audit/audit" class="menulink"><i class="bi bi-book"></i>&nbsp;<?php if(FULL_GUI) { print $text_audit; } ?></a>
        <?php } ?>
          </div>
         </div>
         <div class="col">
          <div class="d-flex align-items-center justify-content-end">
          <?php if(FULL_GUI) { ?>
           <a href="<?php print PATH_PREFIX; ?>settings.php" class="menulink"><i class="bi bi-person pe-2"></i><?php print $realname; ?></a>
          <?php } ?>
           <button id="theme-toggle" class="menulink" title="Toggle Dark Mode" aria-label="Toggle Dark Mode">
             <i class="bi bi-moon-fill" id="theme-icon"></i>
           </button>
           <a href="<?php print PATH_PREFIX; ?>logout.php" class="menulink" title="<?php print $text_logout; ?>"><i class="bi bi-door-open"></i></a>
          </div>
         </div>
       </div>

<?php } ?>
