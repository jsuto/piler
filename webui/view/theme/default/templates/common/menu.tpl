<?php if(Registry::get('username')) { ?>

   <div class="row">

      <div class="leftcell">
         <ul class="dropdown">
            <li class="first_li"><a href="search.php"<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> id="active"<?php } ?>><?php print $text_search; ?></a></li>
            <li class="center_li"><a href="advanced.php"<?php if(strstr($_SERVER['REQUEST_URI'], "advanced.php")){ ?> id="active"<?php } ?>><?php print $text_advanced_search; ?></a></li>
            <li class="last_li"><a href="settings.php"<?php if(strstr($_SERVER['REQUEST_URI'], "settings.php")){ ?> id="active"<?php } ?>><?php print $text_settings; ?></a></li>
         </ul>
      </div>

      <div class="rightcell">
         <div class="logout22">
            <?php if(isset($_SESSION['realname'])) { print $text_realname; ?>: <?php print $_SESSION['realname']; ?>, <?php } ?> <a class="logout" href="index.php?route=login/logout"<?php if(strstr($_SERVER['QUERY_STRING'], "login/logout")){ ?> id="active"<?php } ?>><?php print $text_logout; ?></a>
         </div>
      </div>

   </div>


<?php } ?>


