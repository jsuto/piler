<?php if(Registry::get('username')) { ?>

         <div class="logout22">
            <a href="search.php"<?php if($_SERVER['REQUEST_URI'] == '/' || strstr($_SERVER['REQUEST_URI'], "search.php")){ ?> id="active"<?php } ?>><?php print $text_search; ?></a> | 
            <a href="/settings.php"<?php if(strstr($_SERVER['REQUEST_URI'], "settings.php")){ ?> id="active"<?php } ?>><?php print $text_settings; ?></a> | 
            <?php if(isset($_SESSION['realname'])) { print $text_realname; ?>: <?php print $_SESSION['realname']; ?> | <?php } ?>
            <a href="/logout.php"<?php if(strstr($_SERVER['QUERY_STRING'], "login/logout")){ ?> id="active"<?php } ?>><?php print $text_logout; ?></a>
         </div>

<?php } ?>


