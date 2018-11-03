<?php

define('SITE_NAME_CONST', 'SITE_NAME');

$config[SITE_NAME_CONST] = 'HOSTNAME';
$config['SITE_URL'] = 'http://' . $config[SITE_NAME_CONST] . '/';

$config['SMTP_DOMAIN'] = $config[SITE_NAME_CONST];
$config['SMTP_FROMADDR'] = 'no-reply@' . $config[SITE_NAME_CONST];
$config['ADMIN_EMAIL'] = 'admin@' . $config[SITE_NAME_CONST];

$config['DB_PASSWORD'] = 'MYSQL_PASSWORD';

