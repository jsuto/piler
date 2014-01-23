<?php

$webuidir = "";
$search_expression = "";
$title = "automated search";

$page = 0;
$sort = "date";
$order = 0;
$dry_run = 0;

$opts = 'w:s:dyh';
$lopts = array(
                'webui:',
                'search:',
                'dry-run',
                'yesterday',
                'help'
              );
    
$options = getopt($opts, $lopts);

if(isset($options['help']) || isset($options['h']) )
{
   display_help();
   exit;
}


if(isset($options['webui']))
{
      $webuidir = $options['webui'];
} else
{
      print "\nError: must provide path to WebUI directory\n\n";
      display_help();
      exit;
}
    
if(isset($options['search']))
{
   $search_expression = $options['search'];
} else {
   print "\nError: must provide a search expression\n\n";
   display_help();
   exit;
}

if(isset($options['dry-run']) || isset($options['d']) )
{
   $dry_run = 1;
}


ini_set("session.save_path", "/tmp");

require_once($webuidir . "/config.php");

if(count($automated_search_recipients) < 1)
{
   print "\nError: please set \$automated_search_recipients array in config-site.php, and define the recipients' email addresses\n\n";
   exit;
}


$page_len = MAX_SEARCH_HITS;

$session->set("username", "system");
$session->set("uid", 1);
$session->set("admin_user", 2);
$session->set("email", "system@local");
$session->set("domain", "local");
$session->set("emails", array("system@local"));
$session->set("pagelen", $page_len);

require(DIR_SYSTEM . "/startup.php");

$request = new Request();
Registry::set("request", $request);


Registry::set('document', new Document());


$start = NULL;


$loader = new Loader();
Registry::set('load', $loader);


$language = new Language();
Registry::set('language', $language);

extract($language->data);

if(ENABLE_SYSLOG == 1) { openlog("piler-automated-search", LOG_PID, LOG_MAIL); }


/* check if user has authenticated himself. If not, we send him to login */

Registry::set('username', "system");
Registry::set('admin_user', 0);
Registry::set('auditor_user', 1);
Registry::set('readonly_admin', 0);


$db = new DB(DB_DRIVER, DB_HOSTNAME, DB_USERNAME, DB_PASSWORD, DB_DATABASE, DB_PREFIX);
Registry::set('DB_DATABASE', DB_DATABASE);

Registry::set('db', $db);

Registry::set('DB_DRIVER', DB_DRIVER);

$sphx = new DB(SPHINX_DRIVER, SPHINX_HOSTNAME, "", "", SPHINX_DATABASE, "");
Registry::set('sphx', $sphx);


if(MEMCACHED_ENABLED) {
   $memcache = new Memcache();
   foreach ($memcached_servers as $m){
      $memcache->addServer($m[0], $m[1]);
   }

   Registry::set('memcache', $memcache);
}

Registry::set('counters', $counters);
Registry::set('langs', $langs);
Registry::set('paging', $paging);
Registry::set('themes', $themes);
Registry::set('letters', $letters);
Registry::set('ldap_types', array("AD", "iredmail", "lotus", "zimbra", LDAP_TYPE_GENERIC));
Registry::set('health_smtp_servers', $health_smtp_servers);
Registry::set('partitions_to_monitor', $partitions_to_monitor);
Registry::set('actions', $actions);
Registry::set('import_status', $import_status);


$data = array(
               'page' => 0,
               'sort' => $sort,
               'order' => $order,
               'type' => 'search',
               'search' => $search_expression,
               'searchtype' => 'expert'
             );

$loader->model('search/search');
$loader->model('search/message');
$loader->model('mail/mail');

$search = new ModelSearchSearch();
$mail = new ModelMailMail();

$a = $search->preprocess_post_expert_request($data);

if(isset($options['yesterday']) || isset($options['y']) )
{
   $a['date1'] = $a['date2'] = date("Y.m.d", time() - 86400);
}

list ($n, $total_found, $all_ids, $messages) = $search->search_messages($a, $page);

if($dry_run == 0)
{
   $msg = "From: " . SMTP_FROMADDR . EOL;
   $msg .= "To: " . ADMIN_EMAIL . EOL;
   $msg .= "Subject: =?UTF-8?Q?" . preg_replace("/\n/", "", my_qp_encode($title)) . "?=" . EOL;
   $msg .= "Message-ID: <" . generate_random_string(25) . '@' . SITE_NAME . ">" . EOL;
   $msg .= "MIME-Version: 1.0" . EOL;
   $msg .= "Content-Type: text/html; charset=\"utf-8\"" . EOL;
   $msg .= EOL . EOL;

   ob_start();
   include($webuidir . "/view/theme/default/templates/search/auto.tpl");
   $msg .= ob_get_contents();

   ob_end_clean();

   $x = $mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $automated_search_recipients, $msg);
}
else {
   print "search = $search_expression\n";
   print_r($all_ids);
   print EOL;
}



function display_help() {
    $phpself = basename(__FILE__);

    echo("\nUsage: $phpself --webui [PATH] --search '[SEARCH EXPRESSION]' [OPTIONS...]\n\n");
    echo("\nThe results go to the recipients defined in \$automated_search_recipients, see config-site.php\n\n");

    echo("\t--webui=\"[REQUIRED: path to the piler webui directory]\"\n");
    echo("\t--search=\"[REQUIRED: the search expression]\"\n\n");
    echo("options:\n");
    echo("\t-y | --yesterday: Search \"yesterday\"\n");
    echo("\t-d | --dry-run: Only print the found IDs\n");
    echo("\t-h | --help: Prints this help screen and exits\n");
}


?>
