<?php

$config = array();


/*
 * you can override any of these values by putting the
 * variable to be overridden in config-site.php
 */

$config['PILERGETD_HOST'] = '';
$config['PILERGETD_PORT'] = 10091;
$config['PILERGETD_READ_LENGTH'] = 8192;
$config['PILERGETD_TIMEOUT'] = 5;
$config['PILERGETD_PASSWORD'] = '';

$config['BRANDING_TEXT'] = '';
$config['BRANDING_URL'] = '';
$config['BRANDING_LOGO'] = '';
$config['BRANDING_BACKGROUND_COLOUR'] = '';
$config['BRANDING_TEXT_COLOUR'] = '';
$config['BRANDING_FAVICON'] = '';
$config['SUPPORT_LINK'] = '';
$config['TITLE_PREFIX'] = '';

$config['BOOTSTRAP_THEME'] = '-cosmo';

$config['DEFAULT_LANG'] = 'en';

$config['THEME'] = 'default';

$config['SITE_NAME'] = 'piler.yourdomain.com';
$config['SITE_URL'] = 'http://piler.yourdomain.com/';

$config['ENABLE_SAAS'] = 0;
$config['CAPTCHA_FAILED_LOGIN_COUNT'] = 0;

$config['ENABLE_TABLE_RESIZE'] = 0;
$config['DEMO_MODE'] = 0;

$config['TIMEZONE'] = 'Europe/Budapest';

$config['PROVIDED_BY'] = 'www.mailpiler.org';
$config['SITE_KEYWORDS'] = 'piler email archiver';
$config['SITE_DESCRIPTION'] = 'piler email archiver';

$config['INDEXER_BEACON'] = '/var/piler/stat/indexer';
$config['PURGE_BEACON'] = '/var/piler/stat/purge';

// authentication against an ldap directory (disabled by default)

$config['ENABLE_LDAP_AUTH'] = 0;
$config['LDAP_HOST'] = 'ldap.yourdomain.com';
$config['LDAP_HELPER_DN'] = 'cn=....';
$config['LDAP_HELPER_PASSWORD'] = 'xxxxxxx';
$config['LDAP_MAIL_ATTR'] = 'mail';
$config['LDAP_AUDITOR_MEMBER_DN'] = '';
$config['LDAP_ADMIN_MEMBER_DN'] = '';
$config['LDAP_BASE_DN'] = '';

// AD specific settings
//
$config['LDAP_ACCOUNT_OBJECTCLASS'] = 'user';
$config['LDAP_DISTRIBUTIONLIST_OBJECTCLASS'] = 'group';
$config['LDAP_DISTRIBUTIONLIST_ATTR'] = 'member';

// zimbra specific settings
//$config['LDAP_HELPER_DN'] = 'uid=zimbra,cn=admins,cn=zimbra';
//$config['LDAP_ACCOUNT_OBJECTCLASS'] = 'zimbraAccount';
//$config['LDAP_DISTRIBUTIONLIST_OBJECTCLASS'] = 'zimbraDistributionList';
//$config['LDAP_DISTRIBUTIONLIST_ATTR'] = 'zimbraMailForwardingAddress';

// Lotus Notes specific settings for ldap authentication
//
//$config['LDAP_ACCOUNT_OBJECTCLASS'] = 'dominoPerson';
//$config['LDAP_DISTRIBUTIONLIST_OBJECTCLASS'] = 'dominoGroup');
//$config['LDAP_DISTRIBUTIONLIST_ATTR'] = 'mail';

// iredmail specific settings
//$config['LDAP_HELPER_DN'] = 'cn=vmailadmin,dc=yourdomain,dc=com';
//$config['LDAP_ACCOUNT_OBJECTCLASS'] = 'mailUser';
//$config['LDAP_BASE_DN'] = 'o=domains,dc=yourdomain,dc=com';
//$config['LDAP_DISTRIBUTIONLIST_OBJECTCLASS'] = 'mailList';
//$config['LDAP_DISTRIBUTIONLIST_ATTR'] = 'memberOfGroup';




// enable single sign-on (disabled by default)
$config['ENABLE_SSO_LOGIN'] = 0;

// enable authentication against an imap server (disabled by default)

$config['ENABLE_IMAP_AUTH'] = 0;
$config['IMAP_HOST'] = 'mail.yourdomain.com';
$config['IMAP_PORT'] =  993;
$config['IMAP_SSL'] = true;


// enable authentication against google (disabled by default)
// see http://www.mailpiler.org/en/google-apps-free.html for details

$config['ENABLE_GOOGLE_LOGIN'] = 0;
$config['GOOGLE_CLIENT_ID'] = 'xxxxxxxxxxx';
$config['GOOGLE_CLIENT_SECRET'] = 'xxxxxxxxxxxxx';
$config['GOOGLE_DEVELOPER_KEY'] = 'xxxxxxxxxxxx';
$config['GOOGLE_APPLICATION_NAME'] = 'piler enterprise email archiver';

$config['SITE_LOGO_LG'] = 'view/theme/default/assets/images/archive-logo-lg.png';
$config['SITE_LOGO_SM'] = 'view/theme/default/assets/images/archive-logo-sm.png';
$config['COMPATIBILITY'] = 'Which browsers are supported, etc';


$config['ENABLE_AUDIT'] = 1;
$config['MEMCACHED_ENABLED'] = 0;
$config['PASSWORD_CHANGE_ENABLED'] = 0;
$config['ENABLE_STATISTICS'] = 1;
$config['ENABLE_HISTORY'] = 1;
$config['ENABLE_REMOTE_IMAGES'] = '0';
$config['ENABLE_ON_THE_FLY_VERIFICATION'] = 1;
$config['ENABLE_LDAP_IMPORT_FEATURE'] = 0;
$config['ENABLE_FOLDER_RESTRICTIONS'] = 0;
$config['SEARCH_RESULT_CHECKBOX_CHECKED'] = 0;
$config['HELPER_URL'] = '';
$config['ENABLE_SYSLOG'] = 1;
$config['REWRITE_MESSAGE_ID'] = 0;
$config['RESTRICTED_AUDITOR'] = 0;

$config['SHOW_ENVELOPE_JOURNAL'] = 0;
$config['BULK_DOWNLOAD_FOR_USERS'] = 1;

$config['MAX_CGI_FROM_SUBJ_LEN'] = 34;
$config['PAGE_LEN'] = 20;
$config['MAX_NUMBER_OF_FROM_ITEMS'] = 5;
$config['MAX_SEARCH_HITS'] = 1000;

$config['DEFAULT_RETENTION'] = 0;

$config['LOCALHOST'] = '127.0.0.1';
$config['PILER_HOST'] = '1.2.3.4';
$config['PILER_PORT'] = 25;
$config['SMARTHOST'] = '127.0.0.1';
$config['SMARTHOST_PORT'] = 10026;
$config['SMTP_DOMAIN'] = 'yourdomain.com';
$config['SMTP_FROMADDR'] = 'no-reply@yourdomain.com';
$config['ADMIN_EMAIL'] = 'admin@yourdomain.com';

$config['PILER_HEADER_FIELD'] = 'X-piler-id: ';

$config['DEFAULT_POLICY'] = 'default_policy';

$config['DIR_BASE'] = '/var/www/piler.yourdomain.com/';

$config['DIR_SPHINX'] = '/var/piler/sphinx/';
$config['DIR_STAT'] = '/var/piler/stat';
$config['DIR_IMAP'] = '/var/piler/imap';
$config['DIR_TMP'] = '/var/piler/tmp';

$config['DECRYPT_BINARY'] = '/usr/local/bin/pilerget';
$config['DECRYPT_ATTACHMENT_BINARY'] = '/usr/local/bin/pileraget';
$config['DECRYPT_BUFFER_LENGTH'] = 65536;

$config['DB_DRIVER'] = 'mysql';
$config['DB_PREFIX'] = '';
$config['DB_HOSTNAME'] = 'localhost';
$config['DB_USERNAME'] = 'piler';
$config['DB_PASSWORD'] = 'piler';
$config['DB_DATABASE'] = 'piler';

$config['SPHINX_DRIVER'] = 'sphinx';
$config['SPHINX_DATABASE'] = 'sphinx';
$config['SPHINX_HOSTNAME'] = '127.0.0.1:9306';
$config['SPHINX_MAIN_INDEX'] = 'dailydelta1,main1';
$config['SPHINX_TAG_INDEX'] = 'tag1';
$config['SPHINX_NOTE_INDEX'] = 'note1';


$config['LDAP_IMPORT_CONFIG_FILE'] = '/usr/local/etc/ldap-import.cfg';

$config['DN_MAX_LEN'] = 255;
$config['USE_EMAIL_AS_USERNAME'] = 1;
$config['LDAP_IMPORT_MINIMUM_NUMBER_OF_USERS_TO_HEALTH_OK'] = 100;


$config['PILER_LOGIN_HELPER_PLACEHOLDER'] = 'PILER_COMMENT_FOR_PROPER_LOGIN_SO_THIS_CAN_BE_ANYTHING_JUST_BE_IT_SOMETHING_LIKE_A_UNIQUE_VALUE';


$config['SIZE_X'] = 430;
$config['SIZE_Y'] = 250;

$config['DATE_TEMPLATE'] = 'Y.m.d';
$config['DATE_FORMAT'] = 'YYYY-MM-DD';
$config['JQUERY_DATE_FORMAT'] = 'yy-mm-dd';


$config['FROM_LENGTH_TO_SHOW'] = 28;

$config['HISTORY_REFRESH'] = 60;
$config['HEALTH_REFRESH'] = 60;
$config['HEALTH_RATIO'] = 80;

$config['MAX_AUDIT_HITS'] = 1000;

$config['MIN_PASSWORD_LENGTH'] = 6;

$config['CGI_INPUT_FIELD_WIDTH'] = 50;
$config['CGI_INPUT_FIELD_HEIGHT'] = 7;

$config['MEMCACHED_PREFIX'] = '_piler:';
$config['MEMCACHED_TTL'] = 900;

$memcached_servers = array(
      array('127.0.0.1', 11211)
                          );

$partitions_to_monitor = array('/', '/home', '/var', '/tmp');
$config['DATA_PARTITION'] = '/var';

$config['DELIMITER'] = "\t";

$langs = array(
                'de',
                'en',
                'es',
                'hu',
                'pt'
               );


$themes = array(
                'default',
                'mobile'
               );


$letters = array('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z');


define('NOW', time());


/*
 * normally you don't have to change anything below
 */

require_once 'config-site.php';

if(isset($_SESSION['theme']) && preg_match("/^([a-zA-Z0-9\-\_]+)$/", $_SESSION['theme'])) { $config['THEME'] = $_SESSION['theme']; }

include("system/helper/detectmobilebrowser.php");

if(MOBILE_DEVICE == 1) { $config['THEME'] = 'mobile'; }

// make sure auditors are restricted in a saas environment
if($config['ENABLE_SAAS'] == 1) { $config['RESTRICTED_AUDITOR'] = 1; }
if(isset($_SESSION['username']) && $_SESSION['username'] == 'auditor@local') { $config['RESTRICTED_AUDITOR'] = 0; }



foreach ($config as $k => $v) {
   define($k, $v);
}


define('TABLE_USER', 'user');
define('TABLE_GROUP', 'group');
define('TABLE_GROUP_USER', 'group_user');
define('TABLE_GROUP_EMAIL', 'group_email');
define('TABLE_FOLDER', 'folder');
define('TABLE_FOLDER_USER', 'folder_user');
define('TABLE_FOLDER_EXTRA', 'folder_extra');
define('TABLE_FOLDER_MESSAGE', 'folder_message');
define('TABLE_EMAIL', 'email');
define('TABLE_META', 'metadata');
define('TABLE_RCPT', 'rcpt');
define('TABLE_ATTACHMENT', 'attachment');
define('TABLE_SEARCH', 'search');
define('TABLE_EMAIL_LIST', 'email_groups');
define('TABLE_TAG', 'tag');
define('TABLE_NOTE', '`note`');
define('TABLE_USER_SETTINGS', 'user_settings');
define('TABLE_REMOTE', 'remote');
define('TABLE_DOMAIN', 'domain');
define('TABLE_DOMAIN_USER', 'domain_user');
define('TABLE_COUNTER', 'counter');
define('TABLE_COUNTER_MSG', 'counter_messages');
define('TABLE_STAT_COUNTER', 'counter_stats');
define('TABLE_AUDIT', 'audit');
define('TABLE_ARCHIVING_RULE', 'archiving_rule');
define('TABLE_RETENTION_RULE', 'retention_rule');
define('TABLE_OPTION', 'option');
define('TABLE_LDAP', 'ldap');
define('TABLE_CUSTOMER_SETTINGS', 'customer_settings');
define('TABLE_ONLINE', 'online');
define('TABLE_IMPORT', 'import');
define('TABLE_GOOGLE', 'google');
define('TABLE_GOOGLE_IMAP', 'google_imap');
define('VIEW_MESSAGES', 'v_messages');

define('EOL', "\n");

define('DIR_SYSTEM', DIR_BASE . 'system/');
define('DIR_MODEL', DIR_BASE . 'model/');
define('DIR_DATABASE', DIR_BASE . 'system/database/');
define('DIR_IMAGE', DIR_BASE . 'image/');
define('DIR_LANGUAGE', DIR_BASE . 'language/');
define('DIR_APPLICATION', DIR_BASE . 'controller/');
define('DIR_THEME', DIR_BASE . 'view/theme/');
define('DIR_REPORT', DIR_BASE . 'reports/');
define('DIR_LOG', DIR_BASE . 'log/');

define('REMOTE_IMAGE_REPLACEMENT', '/view/theme/' . THEME . '/images/remote.gif');
define('ICON_ARROW_UP', '/view/theme/' . THEME . '/images/arrowup.gif');
define('ICON_ARROW_DOWN', '/view/theme/' . THEME . '/images/arrowdown.gif');
define('ICON_ATTACHMENT', '/view/theme/' . THEME . '/images/attachment_icon.png');
define('ICON_TAG', '/view/theme/' . THEME . '/images/tag_blue.png');
define('ICON_GREEN_OK', '/view/theme/' . THEME . '/images/green_ok.png');
define('ICON_RED_X', '/view/theme/' . THEME . '/images/red_x.png');
define('ICON_DOWNLOAD', '/view/theme/' . THEME . '/images/download_icon.jpg');
define('ICON_NOTES', '/view/theme/' . THEME . '/images/notes.png');
define('ICON_PLUS', '/view/theme/' . THEME . '/images/plus.gif');
define('ICON_MINUS', '/view/theme/' . THEME . '/images/minus.gif');
define('ICON_EMPTY', '/view/theme/' . THEME . '/images/1x1.gif');

define('LOG_FILE', DIR_LOG . 'webui.log');

define('QSHAPE_ACTIVE_INCOMING', DIR_STAT . '/active+incoming');
define('QSHAPE_ACTIVE_INCOMING_SENDER', DIR_STAT . '/active+incoming-sender');
define('QSHAPE_DEFERRED', DIR_STAT . '/deferred');
define('QSHAPE_DEFERRED_SENDER', DIR_STAT . '/deferred-sender');

define('CPUSTAT', DIR_STAT . '/cpu.stat');
define('AD_SYNC_STAT', DIR_STAT . '/adsync.stat');
define('ARCHIVE_SIZE', DIR_STAT . '/archive.size');
define('LOCK_FILE', DIR_LOG . 'lock');

define('SEARCH_HELPER_URL', SITE_URL . 'search-helper.php');
define('AUDIT_HELPER_URL', SITE_URL . 'audit-helper.php');
define('BULK_RESTORE_URL', SITE_URL . 'bulkrestore.php');
define('SAVE_SEARCH_URL', SITE_URL . 'index.php?route=search/save');
define('LOAD_SAVED_SEARCH_URL', SITE_URL . 'index.php?route=search/load');
define('SEARCH_TAG_URL', SITE_URL . 'index.php?route=search/tag');
define('MESSAGE_NOTE_URL', SITE_URL . 'index.php?route=message/note');
define('GOOGLE_REDIRECT_URL', SITE_URL . 'google.php');
define('HEALTH_URL', SITE_URL . 'index.php?route=health/health');
define('HEALTH_WORKER_URL', SITE_URL . 'index.php?route=health/worker');

define('ACTION_ALL', 0);
define('ACTION_UNKNOWN', 1);
define('ACTION_LOGIN', 2);
define('ACTION_LOGIN_FAILED', 3);
define('ACTION_LOGOUT', 4);
define('ACTION_VIEW_MESSAGE', 5);
define('ACTION_VIEW_HEADER', 6);
define('ACTION_UNAUTHORIZED_VIEW_MESSAGE', 7);
define('ACTION_RESTORE_MESSAGE', 8);
define('ACTION_DOWNLOAD_MESSAGE', 9);
define('ACTION_SEARCH', 10);
define('ACTION_SAVE_SEARCH', 11);
define('ACTION_CHANGE_USER_SETTINGS', 12);
define('ACTION_REMOVE_MESSAGE', 13);
define('ACTION_UNAUTHORIZED_REMOVE_MESSAGE', 14);
define('ACTION_DOWNLOAD_ATTACHMENT', 15);
define('ACTION_UNAUTHORIZED_DOWNLOAD_ATTACHMENT', 16);
define('ACTION_VIEW_JOURNAL', 17);

$actions = array(
                  'unknown' => 1,
                  'login' => 2,
                  'loginfailed' => 3,
                  'logout' => 4,
                  'view' => 5,
                  'view_header' => 6,
                  'restore' => 8,
                  'download' => 9,
                  'search' => 10,
                  'save_search' => 11,
                  'download_attachment' => 15,
                  'journal' => 17
                );


$import_status = array(
                         0 => 'PENDING',
                         1 => 'RUNNING',
                         2 => 'FINISHED',
                         3 => 'ERROR'
                 );


$counters = array(MEMCACHED_PREFIX . 'rcvd', MEMCACHED_PREFIX . 'virus', MEMCACHED_PREFIX . 'duplicate', MEMCACHED_PREFIX . 'ignore', MEMCACHED_PREFIX . 'counters_last_update');

if(!isset($health_smtp_servers)) {
   $health_smtp_servers = array( array(PILER_HOST, PILER_PORT, "piler"), array(SMARTHOST, SMARTHOST_PORT, "smarthost") );
}


?>
