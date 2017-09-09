<?php

$webuidir = "";
$search_expression = "";

$page = 0;
$sort = "date";
$order = 0;
$dry_run = 0;
$auto_search = 0;

$opts = 'w:s:dayh';
$lopts = array(
                'webui:',
                'search:',
                'auto',
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

if(isset($options['auto']) || isset($options['a']) )
{
   $auto_search = 1;
}

if($auto_search == 0)
{
   if(isset($options['search']))
   {
      $search_expression = $options['search'];
   } else {
      print "\nError: must provide a search expression\n\n";
      display_help();
      exit;
   }
}
else {
   if(isset($options['search']))
   {
      print "\nError: don't specify BOTH --search AND --auto\n\n";
      display_help();
      exit;
   }
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



openlog("piler-automated-search", LOG_PID, LOG_MAIL);


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
               'id' => 0,
               'sort' => $sort,
               'order' => $order,
               'type' => 'search',
               'search' => $search_expression,
               'searchtype' => 'expert'
             );

$loader->model('search/search');
$loader->model('search/message');
$loader->model('search/auto');
$loader->model('mail/mail');


if($auto_search == 1)
{
   $sa = new ModelSearchAuto();

   $queries = $sa->get();

   foreach ($queries as $query) {
      $data['search'] = $query['query'];
      $data['id'] = $query['id'];

      do_search($data, $automated_search_recipients);
   }
}
else {
   $data['id'] = '';
   do_search($data, $automated_search_recipients);
}

function do_search($data = array(), $automated_search_recipients = array())
{
   global $options;
   global $dry_run;
   global $webuidir;
   global $search_expression;
   global $page_len;

   $lang = Registry::get('language');
   extract($lang->data);

   $page = 0;
   $title = $text_automated_search;

   if($data['id'] != '') { $title .= $data['id']; }

   $search = new ModelSearchSearch();
   $mail = new ModelMailMail();

   $a = $search->preprocess_post_expert_request($data);

   if(isset($options['yesterday']) || isset($options['y']) )
   {
      $a['date1'] = $a['date2'] = date("Y.m.d", time() - 86400);
   }

   $boundary = "--=_NextPart_000_ABCDEFGHI";

   list ($n, $total_found, $all_ids, $messages) = $search->search_messages($a, 0);

   if($dry_run == 0)
   {
      $msg = "From: " . SMTP_FROMADDR . EOL;
      $msg .= "To: " . ADMIN_EMAIL . EOL;
      $msg .= "Date: " . date(DATE_RFC2822) . EOL;
      $msg .= "Subject: =?UTF-8?Q?" . preg_replace("/\n/", "", my_qp_encode($title)) . "?=" . EOL;
      $msg .= "Message-ID: <" . generate_random_string(25) . '@' . SITE_NAME . ">" . EOL;
      $msg .= "MIME-Version: 1.0" . EOL;
      $msg .= "Content-Type: multipart/alternative;" . EOL;
      $msg .= "\tboundary=\"$boundary\"" . EOL;
      $msg .= EOL . EOL;

      $msg .= "--$boundary" . EOL;
      $msg .= "Content-Type: text/html; charset=\"utf-8\"" . EOL . EOL;

      ob_start();
      include($webuidir . "/view/theme/default/templates/search/auto.tpl");
      $msg .= ob_get_contents();

      $msg .= "--" . $boundary . EOL . EOL;

      ob_end_clean();

      $x = $mail->send_smtp_email(SMARTHOST, SMARTHOST_PORT, SMTP_DOMAIN, SMTP_FROMADDR, $automated_search_recipients, $msg);
   }
   else {
      print "search = " . $data['search'] . "\n";
      print_r($all_ids);
      print EOL . EOL;
   }

}




function display_help() {
    $phpself = basename(__FILE__);

    echo("\nUsage: $phpself [OPTIONS...] --webui [PATH] --search '[SEARCH EXPRESSION]' | --auto\n\n");
    echo("\nThe results go to the recipients defined in \$automated_search_recipients, see config-site.php\n\n");

    echo("\t--webui=\"[REQUIRED: path to the piler webui directory]\"\n");
    echo("\t--search=\"[REQUIRED (unless you specify --auto): the search expression]\"\n\n");
    echo("options:\n");
    echo("\t-a | --auto: Perform an automated search based on queries defined via the piler gui\n");
    echo("\t-y | --yesterday: Search \"yesterday\"\n");
    echo("\t-d | --dry-run: Only print the found IDs\n");
    echo("\t-h | --help: Prints this help screen and exits\n");
}


?>
