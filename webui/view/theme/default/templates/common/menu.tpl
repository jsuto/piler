<?php if(Registry::get('username')) { ?>

   <div class="row">

      <div class="leftcell">
         <div id="ss1">
            <div class="row">
               <div class="mcell" style="width: 80px;">
               </div>
               <div class="mcell" style="width: 315px;">

                  <ul class="dropdown">
                     <li class="search_li"><a href="search.php"<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> id="active"<?php } ?>><?php print $text_simple_search; ?></a></li>
                     <li class="search_li"><a href="advanced.php"<?php if(strstr($_SERVER['REQUEST_URI'], "advanced.php")){ ?> id="active"<?php } ?>><?php print $text_advanced_search; ?></a></li>
                  </ul>
               </div>
               <div class="mcell" style="width: 95px;">
                  <ul class="dropdown">
                     <li class="last_li" style="text-align: right; float: right;"><a href="settings.php"<?php if(strstr($_SERVER['REQUEST_URI'], "settings.php")){ ?> id="active"<?php } ?>><?php print $text_settings; ?></a></li>
                  </ul>
               </div>
            </div>
         </div>

      </div>

      <div class="rightcell">
         <div class="logout22">
            <?php if(isset($_SESSION['realname'])) { print $text_realname; ?>: <?php print $_SESSION['realname']; ?>, <?php } ?> <a class="logout" href="logout.php"<?php if(strstr($_SERVER['QUERY_STRING'], "login/logout")){ ?> id="active"<?php } ?>><?php print $text_logout; ?></a>
         </div>
      </div>

   </div>


<?php } ?>


