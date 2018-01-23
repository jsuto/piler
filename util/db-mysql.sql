
create table if not exists `sph_counter` (
  `counter_id` bigint not null,
  `max_doc_id` bigint not null,
  primary key (`counter_id`)
);


create table if not exists `sph_index` (
  `id` bigint not null,
  `from` char(255) default null,
  `to` text(8192) default null,
  `fromdomain` char(255) default null,
  `todomain` text(512) default null,
  `subject` blob(512) default null,
  `arrived` int unsigned not null,
  `sent` int unsigned not null,
  `body` mediumblob,
  `size` int default '0',
  `direction` int default 0,
  `folder` int default 0,
  `attachments` int default 0,
  `attachment_types` text(512) default null,
  primary key (`id`)
) Engine=InnoDB;


create table if not exists `metadata` (
  `id` bigint unsigned not null auto_increment,
  `from` varchar(128) not null,
  `fromdomain` varchar(64) not null,
  `subject` blob(512) default null,
  `spam` tinyint(1) default 0,
  `arrived` int unsigned not null,
  `sent` int unsigned not null,
  `retained` int unsigned not null,
  `deleted` tinyint(1) default 0,
  `size` int default 0,
  `hlen` int default 0,
  `direction` int default 0,
  `attachments` int default 0,
  `piler_id` char(36) not null,
  `message_id` varchar(255) not null,
  `reference` char(64) not null,
  `digest` char(64) not null,
  `bodydigest` char(64) not null,
  `vcode` char(64) default null,
  primary key (`id`)
) Engine=InnoDB;

create index metadata_idx on metadata(`piler_id`);
create index metadata_idx2 on metadata(`message_id`); 
create index metadata_idx3 on metadata(`reference`); 
create index metadata_idx4 on metadata(`bodydigest`); 
create index metadata_idx5 on metadata(`deleted`); 
create index metadata_idx6 on metadata(`arrived`); 
create index metadata_idx7 on metadata(`retained`); 
create index metadata_idx8 on metadata(`fromdomain`);
create index metadata_idx9 on metadata(`from`);
create index metadata_idx10 on metadata(`sent`);


create table if not exists `rcpt` (
   `id` bigint unsigned not null,
   `to` varchar(128) not null,
   `todomain` varchar(128) not null,
   unique(`id`,`to`)
) Engine=InnoDB;

create index `rcpt_idx` on `rcpt`(`id`);
create index `rcpt_idx2` on `rcpt`(`to`);
create index `rcpt_idx3` on `rcpt`(`todomain`);


drop view if exists `v_messages`;
create view `v_messages` AS select `metadata`.`id` AS `id`,`metadata`.`piler_id` AS `piler_id`,`metadata`.`from` AS `from`,`metadata`.`fromdomain` AS `fromdomain`,`rcpt`.`to` AS `to`,`rcpt`.`todomain` AS `todomain`,`metadata`.`subject` AS `subject`, `metadata`.`size` AS `size`, `metadata`.`direction` AS `direction`, `metadata`.`sent` AS `sent`, `metadata`.`retained` AS `retained`, `metadata`.`arrived` AS `arrived`, `metadata`.`digest` AS `digest`, `metadata`.`bodydigest` AS `bodydigest`, `metadata`.`deleted` AS `deleted` from (`metadata` join `rcpt`) where (`metadata`.`id` = `rcpt`.`id`);


create table if not exists `attachment` (
   `id` bigint unsigned not null auto_increment,
   `piler_id` char(36) not null,
   `attachment_id` int not null,
   `name` tinyblob default null,
   `type` varchar(128) default null,
   `sig` char(64) not null,
   `size` int default 0,
   `ptr` bigint unsigned default 0,
   `deleted` tinyint(1) default 0,
   primary key (`id`)
) Engine=InnoDB;

create index `attachment_idx` on `attachment`(`piler_id`);
create index `attachment_idx2` on `attachment`(`sig`, `size`, `ptr`);
create index `attachment_idx3` on `attachment`(`ptr`);

drop view if exists `v_attachment`;
create view `v_attachment` AS select `id` as `i`, `piler_id`, `attachment_id`, `ptr`, (select count(*) from `attachment` where `ptr`=`i`) as `refcount` from `attachment`;


create table if not exists `tag` (
   `_id` bigint unsigned auto_increment not null,
   `id` bigint not null,
   `uid` int not null,
   `tag` varchar(255) default null,
   unique(`id`, `uid`),
   key (`_id`)
) ENGINE=InnoDB;


create table if not exists `archiving_rule` (
   `id` bigint unsigned not null auto_increment,
   `domain` varchar(128) default null,
   `from` varchar(128) default null,
   `to` varchar(128) default null,
   `subject` varchar(128) default null,
   `body` varchar(128) default null,
   `_size` char(2) default null,
   `size` int default 0,
   `attachment_name` varchar(128) default null,
   `attachment_type` varchar(64) default null,
   `_attachment_size` char(2) default null,
   `attachment_size` int default 0,
   `spam` tinyint(1) default -1,
   `days` int default 0,
   `folder_id` int default 0,
   primary key (`id`),
   unique(`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`) 
) ENGINE=InnoDB;


create table if not exists `retention_rule` (
   `id` bigint unsigned not null auto_increment,
   `domain` varchar(100) default null,
   `from` varchar(100) default null,
   `to` varchar(100) default null,
   `subject` varchar(128) default null,
   `body` varchar(128) default null,
   `_size` char(2) default null,
   `size` int default 0,
   `attachment_name` varchar(100) default null,
   `attachment_type` varchar(64) default null,
   `_attachment_size` char(2) default null,
   `attachment_size` int default 0,
   `spam` tinyint(1) default -1,
   `days` int default 0,
   `folder_id` int default 0,
   primary key (`id`),
   unique (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`)
) ENGINE=InnoDB;


create table if not exists `folder_rule` (
   `id` bigint unsigned not null auto_increment,
   `domain` varchar(100) default null,
   `from` varchar(100) default null,
   `to` varchar(100) default null,
   `subject` varchar(128) default null,
   `body` varchar(128) default null,
   `_size` char(2) default null,
   `size` int default 0,
   `attachment_name` varchar(128) default null,
   `attachment_type` varchar(64) default null,
   `_attachment_size` char(2) default null,
   `attachment_size` int default 0,
   `spam` tinyint(1) default -1,
   `days` int default 0,
   `folder_id` int default 0,
   primary key (`id`),
   unique (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`)
) ENGINE=InnoDB;


create table if not exists `counter` (
   `rcvd` bigint unsigned default 0,
   `virus` bigint unsigned default 0,
   `duplicate` bigint unsigned default 0,
   `ignore` bigint unsigned default 0,
   `size` bigint unsigned default 0,
   `stored_size` bigint unsigned default 0
) Engine=InnoDB;

insert into `counter` values(0, 0, 0, 0, 0, 0);


create table if not exists `option` (
   `key` char(64) not null,
   `value` char(32) not null
) Engine=InnoDB;

insert into `option` (`key`, `value`) values('enable_purge', '1');


create table if not exists `search` (
   `email` char(128) not null,
   `ts` int default 0,
   `term` text(512) not null
) Engine=InnoDB;

create index `search_idx` on `search`(`email`);


create table if not exists `user_settings` (
   `username` char(64) not null unique,
   `pagelen` int default 20,
   `theme` char(8) default 'default',
   `lang` char(2) default null,
   `ga_enabled` int default 0,
   `ga_secret` varchar(255) default null
) Engine=InnoDB;

create index `user_settings_idx` on `user_settings`(`username`);


create table if not exists `user` (
   `uid` int unsigned not null primary key,
   `username` char(64) not null unique,
   `realname` char(64) default null,
   `samaccountname` char(64) default null,
   `password` char(128) default null,
   `domain` char(64) default null,
   `dn` char(255) default '*',
   `isadmin` tinyint default 0
) Engine=InnoDB;

insert into `user` (`uid`, `username`, `realname`, `samaccountname`, `password`, `isadmin`, `domain`) values (0, 'admin', 'built-in piler admin', '', '$1$PItc7d$zsUgON3JRrbdGS11t9JQW1', 1, 'local');
insert into `user` (`uid`, `username`, `realname`, `samaccountname`, `password`, `isadmin`, `domain`) values (1, 'auditor', 'built-in piler auditor', '', '$1$SLIIIS$JMBwGqQg4lIir2P2YU1y.0', 2, 'local');

create table if not exists `email` (
   `uid` int unsigned not null,
   `email` char(128) not null primary key
) ENGINE=InnoDB;

insert into `email` (`uid`, `email`) values(0, 'admin@local');
insert into `email` (`uid`, `email`) values(1, 'auditor@local');


create table if not exists `email_groups` (
   `uid` int unsigned not null,
   `gid` int unsigned not null,
   unique key `uid` (`uid`,`gid`),
   key `email_groups_idx` (`uid`,`gid`)
) ENGINE=InnoDB;


create table if not exists `group` (
   `id` bigint unsigned not null auto_increment primary key,
   `groupname` char(128) not null unique
) ENGINE=InnoDB;


create table if not exists `group_user` (
   `id` bigint unsigned not null,
   `email` char(128) not null,
   key `group_user_idx` (`id`),
   key `group_user_idx2` (`email`)
) ENGINE=InnoDB;


create table if not exists `group_email` (
   `id` bigint unsigned not null,
   `email` char(128) not null,
   key `group_email_idx` (`id`)
) ENGINE=InnoDB;


create table if not exists `domain_user` (
   `domain` char(64) not null,
   `uid` int unsigned not null,
   key `domain_user_idx` (`domain`),
   key `domain_user_idx2` (`uid`)
) ENGINE=InnoDB;


create table if not exists `folder` (
   `id` int not null auto_increment,
   `parent_id` int default 0,
   `name` char(64) not null,
   unique(`parent_id`, `name`),
   primary key (`id`)
) Engine=InnoDB;


create table if not exists `folder_user` (
   `id` bigint unsigned not null,
   `uid` int unsigned not null,
   key `folder_user_idx` (`id`),
   key `folder_user_idx2` (`uid`)
) ENGINE=InnoDB;


create table if not exists `folder_extra` (
   `id` int unsigned not null auto_increment,
   `uid` int unsigned not null,
   `name` char(64) not null,
   unique(uid, name),
   key (`id`)
) ENGINE=InnoDB;


create table if not exists `folder_message` (
   folder_id bigint not null,
   id bigint not null,
   unique(folder_id, id)
) ENGINE=InnoDB;


create table if not exists `note` (
   `_id` bigint unsigned auto_increment not null,
   `id` bigint unsigned not null,
   `uid` int not null,
   `note` text default null,
   unique(`id`, `uid`),
   key (`_id`)
) ENGINE=InnoDB;


create table if not exists `private` (
   `id` bigint unsigned not null unique,
   key (`id`)
) ENGINE=InnoDB;


create table if not exists `remote` (
   `remotedomain` char(64) not null primary key,
   `remotehost` char(64) not null,
   `basedn` char(255) not null,
   `binddn` char(255) not null,
   `sitedescription` char(64) default null
) ENGINE=InnoDB;


create table if not exists `domain` (
   `domain` char(64) not null primary key,
   `mapped` char(64) not null,
   `ldap_id` int default 0
) ENGINE=InnoDB;

insert into `domain` (`domain`, `mapped`) values('local', 'local');


create table if not exists `audit` (
   `id` bigint unsigned not null auto_increment,
   `ts` int not null,
   `email` varchar(128) not null,
   `domain` varchar(128) not null,
   `action` int not null,
   `ipaddr` char(15) not null,
   `meta_id` bigint unsigned not null,
   `description` varchar(255) default null,
   `vcode` char(64) default null,
   primary key (`id`)
) ENGINE=InnoDB;

create index `audit_idx` on `audit`(`email`);
create index `audit_idx2` on `audit`(`action`);
create index `audit_idx3` on `audit`(`ipaddr`);
create index `audit_idx4` on `audit`(`ts`);
create index `audit_idx5` on `audit`(`domain`);



create table if not exists `google` (
   `id` char(32) not null primary key,
   `email` char(128) not null unique,
   `access_token` char(255) default null,
   `refresh_token` char(255) default null,
   `created` int default 0
) ENGINE=InnoDB;


create table if not exists `google_imap` (
   `id` char(32) not null primary key,
   `email` char(128) not null,
   `last_msg_id` bigint default 0,
   key(`email`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `counter_stats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date` int(11) NOT NULL,
  `email` varchar(255) NOT NULL,
  `domain` varchar(255) NOT NULL,
  `sent` int(11) NOT NULL,
  `recd` int(11) NOT NULL,
  `sentsize` int(11) NOT NULL,
  `recdsize` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date` (`date`),
  KEY `email` (`email`),
  KEY `domain` (`domain`)
) ENGINE=InnoDB;


create table if not exists `ldap` (
   `id` int not null auto_increment primary key,
   `description` varchar(255) not null,
   `ldap_type` varchar(255) not null,
   `ldap_host` varchar(255) not null,
   `ldap_base_dn` varchar(255) not null,
   `ldap_bind_dn` varchar(255) not null,
   `ldap_bind_pw` varchar(255) not null,
   `ldap_auditor_member_dn` varchar(255) default null,
   `ldap_mail_attr` varchar(128) default null,
   `ldap_account_objectclass` varchar(128) default null,
   `ldap_distributionlist_attr` varchar(128) default null,
   `ldap_distributionlist_objectclass` varchar(128) default null
) Engine=InnoDB;


create table if not exists `customer_settings` (
   `id` int not null auto_increment primary key,
   `domain` varchar(128) not null unique,
   `branding_text` varchar(255) default null,
   `branding_url` varchar(255) default null,
   `branding_logo` varchar(255) default null,
   `support_link` varchar(255) default null,
   `background_colour` varchar(255) default null,
   `text_colour` varchar(255) default null
) Engine=InnoDB;


create table if not exists `online` (
   `username` varchar(128) not null,
   `ts` int default 0,
   `last_activity` int default 0,
   `ipaddr` varchar(64) default null,
   unique(`username`,`ipaddr`)
) Engine=InnoDB;


create table if not exists `import` (
   `id` int not null auto_increment primary key,
   `type` varchar(255) not null,
   `username` varchar(255) not null,
   `password` varchar(255) not null,
   `server` varchar(255) not null,
   `created` int default 0,
   `started` int default 0,
   `finished` int default 0,
   `updated` int default 0,
   `status` int default 0,
   `total` int default 0,
   `imported` int default 0,
   `duplicate` int default 0,
   `error` int default 0,
   `cleared` int default 0
) Engine=InnoDB;


create table if not exists `autosearch` (
   `id` int not null auto_increment primary key,
   `query` varchar(512) not null
) Engine=InnoDB;


create table if not exists `legal_hold` (
   email varchar(128) unique not null
) Engine=InnoDB;


create table if not exists `timestamp` (
  `id` bigint unsigned not null auto_increment,
  `start_id` bigint default 0,
  `stop_id` bigint default 0,
  `hash_value` char(40),
  `count` int default 0,
  `response_time` bigint default 0,
  `response_string` blob not null,
  primary key (`id`)
) Engine=InnoDB;

