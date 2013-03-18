<?php

$config = array();


/*
 * you can override any of these values by putting the
 * variable to be overridden in config-site.php
 */



$config['DEFAULT_LANG'] = 'en';

$config['THEME'] = 'default';

$config['SITE_NAME'] = 'piler.yourdomain.com';
$config['SITE_URL'] = 'http://piler.yourdomain.com/';

$config['ENABLE_AUDIT'] = 1;
$config['MEMCACHED_ENABLED'] = 0;
$config['PASSWORD_CHANGE_ENABLED'] = 0;
$config['ENABLE_STATISTICS'] = 1;
$config['ENABLE_HISTORY'] = 1;
$config['ENABLE_REMOTE_IMAGES'] = '0';
$config['ENABLE_ON_THE_FLY_VERIFICATION'] = 1;
$config['ENABLE_LDAP_IMPORT_FEATURE'] = 0;
$config['ENABLE_FOLDER_RESTRICTIONS'] = 0;
$config['ENABLE_GOOGLE_LOGIN'] = 0;
$config['SEARCH_RESULT_CHECKBOX_CHECKED'] = 1;
$config['HELPER_URL'] = '';
$config['ENABLE_SYSLOG'] = 0;
$config['REWRITE_MESSAGE_ID'] = 0;
$config['RESTRICTED_AUDITOR'] = 0;
$config['GOOGLE_CLIENT_ID'] = 'xxxxxxxxxxx';
$config['GOOGLE_CLIENT_SECRET'] = 'xxxxxxxxxxxxx';
$config['GOOGLE_DEVELOPER_KEY'] = 'xxxxxxxxxxxx';
$config['GOOGLE_APPLICATION_NAME'] = 'piler enterprise email archiver';
$config['ENABLE_IMAP_AUTH'] = 0;
$config['IMAP_HOST'] = 'mail.yourdomain.com';
$config['IMAP_PORT'] =  993;
$config['IMAP_SSL'] = true;

$config['MAX_CGI_FROM_SUBJ_LEN'] = 45;
$config['PAGE_LEN'] = 20;
$config['MAX_NUMBER_OF_FROM_ITEMS'] = 5;
$config['MAX_SEARCH_HITS'] = 1000;

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
$config['SPHINX_MAIN_INDEX'] = 'main1';
$config['SPHINX_TAG_INDEX'] = 'tag1';
$config['SPHINX_NOTE_INDEX'] = 'note1';


$config['LDAP_IMPORT_CONFIG_FILE'] = '/usr/local/etc/ldap-import.cfg';

$config['DN_MAX_LEN'] = 255;
$config['USE_EMAIL_AS_USERNAME'] = 1;
$config['LDAP_IMPORT_MINIMUM_NUMBER_OF_USERS_TO_HEALTH_OK'] = 100;


$config['ENABLE_LDAP_AUTH'] = 0;
$config['LDAP_HOST'] = 'zimbra.yourdomain.com';
$config['LDAP_HELPER_DN'] = 'uid=zimbra,cn=admins,cn=zimbra';
$config['LDAP_HELPER_PASSWORD'] = 'xxxxxxx';
$config['LDAP_MAIL_ATTR'] = 'mail';
$config['LDAP_ACCOUNT_OBJECTCLASS'] = 'zimbraAccount';
$config['LDAP_BASE_DN'] = '';
$config['LDAP_DISTRIBUTIONLIST_OBJECTCLASS'] = 'zimbraDistributionList';
$config['LDAP_DISTRIBUTIONLIST_ATTR'] = 'zimbraMailForwardingAddress';

$config['SIZE_X'] = 430;
$config['SIZE_Y'] = 250;

$config['AUDIT_DATE_FORMAT'] = 'Y.m.d H:i';
$config['SEARCH_HIT_DATE_FORMAT'] = 'Y.m.d';

$config['DATE_FORMAT'] = '(Y.m.d.)';
$config['TIMEZONE'] = 'Europe/Budapest';

$config['FROM_LENGTH_TO_SHOW'] = 28;

$config['HISTORY_REFRESH'] = 60;
$config['HEALTH_REFRESH'] = 60;
$config['HEALTH_RATIO'] = 80;

$config['LOG_DATE_FORMAT'] = 'd-M-Y H:i:s';

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

$langs = array(
                'hu',
                'en',
                'pt'
               );


$themes = array(
                'default'
               );


$letters = array('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z');


define('NOW', time());


/*
 * normally you don't have to change anything below
 */

require_once 'config-site.php';

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

define('REMOTE_IMAGE_REPLACEMENT', '/view/theme/default/images/remote.gif');
define('ICON_ARROW_UP', '/view/theme/default/images/arrowup.gif');
define('ICON_ARROW_DOWN', '/view/theme/default/images/arrowdown.gif');
define('ICON_ATTACHMENT', '/view/theme/default/images/attachment_icon.png');
define('ICON_TAG', '/view/theme/default/images/tag_blue.png');
define('ICON_GREEN_OK', '/view/theme/default/images/green_ok.png');
define('ICON_RED_X', '/view/theme/default/images/red_x.png');
define('ICON_DOWNLOAD', '/view/theme/default/images/download_icon.jpg');
define('ICON_NOTES', '/view/theme/default/images/notes.png');
define('ICON_PLUS', '/view/theme/default/images/plus.gif');
define('ICON_MINUS', '/view/theme/default/images/minus.gif');
define('ICON_EMPTY', '/view/theme/default/images/1x1.gif');

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

$actions = array(
                  'unknown' => 1,
                  'login' => 2,
                  'loginfailed' => 3,
                  'logout' => 4,
                  'view' => 5,
                  'restore' => 8,
                  'download' => 9,
                  'search' => 10
                );


$counters = array(MEMCACHED_PREFIX . 'rcvd', MEMCACHED_PREFIX . 'virus', MEMCACHED_PREFIX . 'duplicate', MEMCACHED_PREFIX . 'ignore', MEMCACHED_PREFIX . 'counters_last_update');

$health_smtp_servers = array( array(PILER_HOST, PILER_PORT, "piler"), array(SMARTHOST, SMARTHOST_PORT, "smarthost") );

?>
