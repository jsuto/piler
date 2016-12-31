<?php

$config['SITE_NAME'] = 'HOSTNAME';
$config['SITE_URL'] = 'http://' . $config['SITE_NAME'] . '/';

$config['SMTP_DOMAIN'] = $config['SITE_NAME'];
$config['SMTP_FROMADDR'] = 'no-reply@' . $config['SITE_NAME'];
$config['ADMIN_EMAIL'] = 'admin@' . $config['SITE_NAME'];

$config['DB_PASSWORD'] = 'MYSQL_PASSWORD';

