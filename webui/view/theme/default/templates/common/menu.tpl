<?php if(Registry::get('username')) { ?>

       <div id="menu" class="row w-100">
         <div class="col">
          <div class="d-flex align-items-center">

           <a class="menulink"<?php if($settings['branding_url']) { ?> target="_blank" href="<?php print $settings['branding_url']; ?>" title="<?php print $settings['branding_text']; ?>"<?php } ?>><?php if($settings['branding_logo']) { ?><img src="<?php print $settings['branding_logo']; ?>" alt="<?php print $settings['branding_text']; ?>"/><?php } ?></a>

           <a href="#" class="menulink"><i class="bi bi-search"></i><?php print $text_search; ?></a>
           <a href="#" class="menulink"><i class="bi bi-question-circle pe-2"></i><?php print $text_help; ?></a>
          </div>
         </div>
         <div class="col">
          <div class="d-flex align-items-center justify-content-end">
           <a href="#" class="menulink"><i class="bi bi-person pe-2"></i><?php print $realname; ?></a>
           <a href="/logout.php" class="menulink" title="<?php print $text_logout; ?>"><i class="bi bi-door-open"></i></a>
          </div>
         </div>
       </div>

<?php } ?>
