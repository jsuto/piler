<?php if(Registry::get('username')) { ?>

   <div class="row">

      <div class="leftcell">
         <ul class="dropdown">
            <li class="first_li_admin"><a href="search.php"<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> id="active"<?php } ?>><?php print $text_search; ?></a></li>
            <li class="admin"><a href="advanced.php"<?php if(strstr($_SERVER['REQUEST_URI'], "advanced.php")){ ?> id="active"<?php } ?>><?php print $text_advanced_search; ?></a></li>

            <li class="admin"><a class="hide" href="#"<?php if(strstr($_SERVER['QUERY_STRING'], "stat/") || strstr($_SERVER['QUERY_STRING'], "health/") || strstr($_SERVER['QUERY_STRING'], "audit/") ) { ?> id="active"<?php } ?>><?php print $text_monitor; ?></a>
               <ul class="sub_menu">
                  <li><a href="index.php?route=stat/stat&timespan=daily"><?php print $text_statistics; ?></a></li>
                  <li><a href="index.php?route=health/health"><?php print $text_health; ?></a></li>
                  <li><a href="index.php?route=audit/audit"><?php print $text_audit; ?></a></li>
               </ul>
            </li>

            <li class="admin"><a class="hide" href="#"<?php if(strstr($_SERVER['QUERY_STRING'], "domain/") || ($_SERVER['QUERY_STRING'] != "route=user/settings" && strstr($_SERVER['QUERY_STRING'], "user/")) || strstr($_SERVER['QUERY_STRING'], "policy/") || strstr($_SERVER['QUERY_STRING'], "import/")) { ?> id="active"<?php } ?>><?php print $text_administration; ?></a>
               <ul class="sub_menu">
                  <li><a href="index.php?route=user/list"><?php print $text_user_management; ?></a></li>
                  <li><a href="index.php?route=domain/domain"><?php print $text_domain; ?></a></li>
                  <li><a href="index.php?route=policy/archiving"><?php print $text_archiving_rules; ?></a></li>
                  <li><a href="index.php?route=policy/retention"><?php print $text_retention_rules; ?></a></li>
               </ul>
            </li>

            <li class="last_li_admin"><a href="settings.php"<?php if(strstr($_SERVER['REQUEST_URI'], "settings.php")){ ?> id="active"<?php } ?>><?php print $text_settings; ?></a></li>

         </ul>
      </div>

      <div class="rightcell">
         <div class="logout22">
            <?php if(isset($_SESSION['realname'])) { print $text_realname; ?>: <?php print $_SESSION['realname']; ?>, <?php } ?> <a class="logout" href="index.php?route=login/logout"<?php if(strstr($_SERVER['QUERY_STRING'], "login/logout")){ ?> id="active"<?php } ?>><?php print $text_logout; ?></a>
         </div>
      </div>


   </div>


<?php } ?>
