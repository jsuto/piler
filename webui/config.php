<?php

define('DEFAULT_LANG', 'en');
define('THEME', 'default');

define('SITE_NAME', 'demo.mailpiler.org');
define('SITE_URL', 'http://demo.mailpiler.org/');

define('ENABLE_AUDIT', 1);
define('MEMCACHED_ENABLED', 0);
define('PASSWORD_CHANGE_ENABLED', 0);
define('ENABLE_STATISTICS', 1);
define('ENABLE_HISTORY', 1);
define('ENABLE_REMOTE_IMAGES', '0');
define('ENABLE_ON_THE_FLY_VERIFICATION', 1);
define('ENABLE_LDAP_IMPORT_FEATURE', 0);
define('ENABLE_FOLDER_RESTRICTIONS', 0);
define('ENABLE_GOOGLE_LOGIN', 0);
define('SEARCH_RESULT_CHECKBOX_CHECKED', 1);
define('HELPER_URL', '');
define('ENABLE_SYSLOG', 0);
define('REWRITE_MESSAGE_ID', 0);
define('RESTRICTED_AUDITOR', 0);
define('GOOGLE_CLIENT_ID', 'xxxxxxxxxxx');
define('GOOGLE_CLIENT_SECRET', 'xxxxxxxxxxxxx');
define('GOOGLE_DEVELOPER_KEY', 'xxxxxxxxxxxx');
define('GOOGLE_APPLICATION_NAME', 'piler enterprise email archiver');

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

define('MAX_CGI_FROM_SUBJ_LEN', 45);
define('PAGE_LEN', 20);
define('MAX_NUMBER_OF_FROM_ITEMS', 5);
define('MAX_SEARCH_HITS', 1000);

define('LOCALHOST', '127.0.0.1');
define('PILER_HOST', '1.2.3.4');
define('PILER_PORT', 25);
define('SMARTHOST', '127.0.0.1');
define('SMARTHOST_PORT', 10026);
define('SMTP_DOMAIN', 'mailpiler.org');
define('SMTP_FROMADDR', 'no-reply@mailpiler.org');
define('ADMIN_EMAIL', 'admin@mailpiler.org');

define('EOL', "\n");

define('PILER_HEADER_FIELD', 'X-piler-id: ');

define('DEFAULT_POLICY', 'default_policy');

define('DIR_BASE', '/var/www/demo.mailpiler.org/');
define('DIR_SYSTEM', DIR_BASE . 'system/');
define('DIR_MODEL', DIR_BASE . 'model/');
define('DIR_DATABASE', DIR_BASE . 'system/database/');
define('DIR_IMAGE', DIR_BASE . 'image/');
define('DIR_LANGUAGE', DIR_BASE . 'language/');
define('DIR_APPLICATION', DIR_BASE . 'controller/');
define('DIR_THEME', DIR_BASE . 'view/theme/');
define('DIR_REPORT', DIR_BASE . 'reports/');
define('DIR_LOG', DIR_BASE . 'log/');

define('DIR_STORE', '/var/piler/store');
define('DIR_STAT', '/var/piler/stat');
define('DIR_IMAP', '/var/piler/imap');
define('DIR_TMP', '/var/piler/tmp');

define('DECRYPT_BINARY', '/usr/local/bin/pilerget');
define('DECRYPT_ATTACHMENT_BINARY', '/usr/local/bin/pileraget');
define('DECRYPT_BUFFER_LENGTH', 65536);

define('QSHAPE_ACTIVE_INCOMING', DIR_STAT . '/active+incoming');
define('QSHAPE_ACTIVE_INCOMING_SENDER', DIR_STAT . '/active+incoming-sender');
define('QSHAPE_DEFERRED', DIR_STAT . '/deferred');
define('QSHAPE_DEFERRED_SENDER', DIR_STAT . '/deferred-sender');

define('CPUSTAT', DIR_STAT . '/cpu.stat');
define('AD_SYNC_STAT', DIR_STAT . '/adsync.stat');
define('ARCHIVE_SIZE', DIR_STAT . '/archive.size');
define('LOCK_FILE', DIR_LOG . 'lock');

define('DB_DRIVER', 'mysql');
define('DB_PREFIX', '');
define('DB_HOSTNAME', 'localhost');
define('DB_USERNAME', 'piler');
define('DB_PASSWORD', 'piler');
define('DB_DATABASE', 'piler');

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
define('TABLE_ATTACHMENT', 'attachment');
define('TABLE_SEARCH', 'search');
define('TABLE_EMAIL_LIST', 'email_groups');
define('TABLE_TAG', 'tag');
define('TABLE_NOTE', '`note`');
define('TABLE_USER_SETTINGS', 'user_settings');
define('TABLE_REMOTE', 'remote');
define('TABLE_DOMAIN', 'domain');
define('TABLE_COUNTER', 'counter');
define('TABLE_AUDIT', 'audit');
define('TABLE_ARCHIVING_RULE', 'archiving_rule');
define('TABLE_RETENTION_RULE', 'retention_rule');
define('TABLE_OPTION', 'option');
define('TABLE_GOOGLE', 'google');
define('TABLE_GOOGLE_IMAP', 'google_imap');
define('VIEW_MESSAGES', 'v_messages');

define('SPHINX_DRIVER', 'sphinx');
define('SPHINX_DATABASE', 'sphinx');
define('SPHINX_HOSTNAME', '127.0.0.1:9306');
define('SPHINX_MAIN_INDEX', 'main1');
define('SPHINX_TAG_INDEX', 'tag1');
define('SPHINX_NOTE_INDEX', 'note1');


define('LDAP_IMPORT_CONFIG_FILE', '/usr/local/etc/ldap-import.cfg');

define('DN_MAX_LEN', 255);
define('USE_EMAIL_AS_USERNAME', 1);
define('LDAP_IMPORT_MINIMUM_NUMBER_OF_USERS_TO_HEALTH_OK', 100);

define('PREVIEW_WIDTH', 1024);
define('PREVIEW_HEIGTH', 700);

define('SIZE_X', 430);
define('SIZE_Y', 250);

define('HELPURL', '');

define('AUDIT_DATE_FORMAT', 'Y.m.d H:i');
define('SEARCH_HIT_DATE_FORMAT', 'Y.m.d');

define('DATE_FORMAT', '(Y.m.d.)');
define('TIMEZONE', 'Europe/Budapest');

define('HISTORY_REFRESH', 60);

define('FROM_LENGTH_TO_SHOW', 28);

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
define('HEALTH_REFRESH', 60);
define('HEALTH_RATIO', 80);

define('LOG_FILE', DIR_LOG . 'webui.log');
define('LOG_DATE_FORMAT', 'd-M-Y H:i:s');

define('MAX_AUDIT_HITS', 1000);

define('MIN_PASSWORD_LENGTH', 6);

define('CGI_INPUT_FIELD_WIDTH', 50);
define('CGI_INPUT_FIELD_HEIGHT', 7);

define('MEMCACHED_PREFIX', '_piler:');
define('MEMCACHED_TTL', 600);

$memcached_servers = array(
      array('127.0.0.1', 11211)
                          );

$counters = array(MEMCACHED_PREFIX . 'rcvd', MEMCACHED_PREFIX . 'virus', MEMCACHED_PREFIX . 'duplicate', MEMCACHED_PREFIX . 'ignore', MEMCACHED_PREFIX . 'counters_last_update');

$health_smtp_servers = array( array(PILER_HOST, PILER_PORT, "piler"), array(SMARTHOST, SMARTHOST_PORT, "smarthost") );

$partitions_to_monitor = array('/', '/home', '/var', '/tmp');


$langs = array(
                'hu',
                'en',
                'pt'
               );


$themes = array(
                'default'
               );


$letters = array('A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z');


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


define('NOW', time());

define('SIMPLE_SEARCH', 0);
define('ADVANCED_SEARCH', 1);
define('EXPERT_SEARCH', 2);

?>
