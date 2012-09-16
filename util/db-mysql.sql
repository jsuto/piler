create database if not exists `piler` character set 'utf8';
use `piler`;


drop table if exists `sph_counter`;
create table if not exists `sph_counter` (
  `counter_id` int not null,
  `max_doc_id` int not null,
  primary key (`counter_id`)
);


drop table if exists `sph_index`;
create table if not exists `sph_index` (
  `id` bigint not null,
  `from` char(255) default null,
  `to` text(8192) default null,
  `fromdomain` char(255) default null,
  `todomain` text(512) default null,
  `subject` text(512) default null,
  `arrived` int unsigned not null,
  `sent` int unsigned not null,
  `body` text,
  `size` int default '0',
  `direction` int default 0,
  `folder` int default 0,
  `attachments` int default 0,
  `attachment_types` text(512) default null,
  primary key (`id`)
) Engine=InnoDB;


drop table if exists `metadata`;
create table if not exists `metadata` (
  `id` bigint unsigned not null auto_increment,
  `from` char(255) not null,
  `fromdomain` char(48) not null,
  `subject` text(512) default null,
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
  `message_id` char(128) character set 'latin1' not null,
  `reference` char(64) character set 'latin1' not null,
  `digest` char(64) not null,
  `bodydigest` char(64) not null,
  `vcode` char(64) default null,
  primary key (`id`), unique(`message_id`)
) Engine=InnoDB;

create index metadata_idx on metadata(`piler_id`);
create index metadata_idx2 on metadata(`message_id`); 
create index metadata_idx3 on metadata(`reference`); 
create index metadata_idx4 on metadata(`bodydigest`); 
create index metadata_idx5 on metadata(`deleted`); 
create index metadata_idx6 on metadata(`arrived`); 
create index metadata_idx7 on metadata(`retained`); 


drop table if exists `rcpt`;
create table if not exists `rcpt` (
   `id` bigint unsigned not null,
   `to` char(64) not null,
   `todomain` char(48) not null,
   unique(`id`,`to`)
) Engine=InnoDB;

create index `rcpt_idx` on `rcpt`(`id`);
create index `rcpt_idx2` on `rcpt`(`to`);


drop view if exists `v_messages`;
create view `v_messages` AS select `metadata`.`id` AS `id`,`metadata`.`piler_id` AS `piler_id`,`metadata`.`from` AS `from`,`metadata`.`fromdomain` AS `fromdomain`,`rcpt`.`to` AS `to`,`rcpt`.`todomain` AS `todomain`,`metadata`.`subject` AS `subject`, `metadata`.`size` AS `size`, `metadata`.`direction` AS `direction`, `metadata`.`sent` AS `sent`, `metadata`.`digest` AS `digest`, `metadata`.`bodydigest` AS `bodydigest` from (`metadata` join `rcpt`) where (`metadata`.`id` = `rcpt`.`id`);


drop table if exists `attachment`;
create table if not exists `attachment` (
   `id` bigint unsigned not null auto_increment,
   `piler_id` char(36) not null,
   `attachment_id` int not null,
   `name` char(64) default null,
   `type` char(72) default null,
   `sig` char(64) not null,
   `size` int default 0,
   `ptr` int default 0,
   `deleted` tinyint(1) default 0,
   primary key (`id`)
) Engine=InnoDB;

create index `attachment_idx` on `attachment`(`piler_id`);
create index `attachment_idx2` on `attachment`(`sig`);
create index `attachment_idx3` on `attachment`(`ptr`);

drop view if exists `v_attachment`;
create view `v_attachment` AS select `id` as `i`, `piler_id`, `attachment_id`, `ptr`, (select count(*) from `attachment` where `ptr`=`i`) as `refcount` from `attachment`;


drop table if exists `tag`;
create table if not exists `tag` (
   `_id` bigint unsigned auto_increment not null,
   `id` bigint not null,
   `uid` int not null,
   `tag` char(255) default null,
   unique(`id`, `uid`),
   key (`_id`)
) ENGINE=InnoDB;


drop table if exists `archiving_rule`;
create table if not exists `archiving_rule` (
   `id` bigint unsigned not null auto_increment,
   `from` char(128) character set 'latin1' default null,
   `to` char(255) character set 'latin1' default null,
   `subject` char(255) character set 'latin1' default null,
   `_size` char(2) default null,
   `size` int default 0,
   `attachment_type` char(128) character set 'latin1' default null,
   `_attachment_size` char(2) default null,
   `attachment_size` int default 0,
   `spam` tinyint(1) default -1,
   `days` int default 0,
   primary key (`id`),
   unique(`from`,`to`,`subject`,`_size`,`size`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`) 
) ENGINE=InnoDB;


drop table if exists `retention_rule`;
create table if not exists `retention_rule` (
   `id` bigint unsigned not null auto_increment,
   `from` char(128) character set 'latin1' default null,
   `to` char(255) character set 'latin1' default null,
   `subject` char(255) character set 'latin1' default null,
   `_size` char(2) default null,
   `size` int default 0,
   `attachment_type` char(128) character set 'latin1' default null,
   `_attachment_size` char(2) default null,
   `attachment_size` int default 0,
   `spam` tinyint(1) default -1,
   `days` int default 0,
   primary key (`id`),
   unique(`from`,`to`,`subject`,`_size`,`size`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`)
) ENGINE=InnoDB;


drop table if exists `counter`;
create table if not exists `counter` (
   `rcvd` bigint unsigned default 0,
   `virus` bigint unsigned default 0,
   `duplicate` bigint unsigned default 0,
   `ignore` bigint unsigned default 0,
   `size` bigint unsigned default 0
) Engine=InnoDB;

insert into `counter` values(0, 0, 0, 0, 0);


drop table if exists `option`;
create table if not exists `option` (
   `key` char(64) not null,
   `value` char(32) not null
) Engine=InnoDB;

insert into `option` (`key`, `value`) values('enable_purge', '1');


drop table if exists `search`;
create table if not exists `search` (
   `email` char(128) not null,
   `ts` int default 0,
   `term` text(512) not null
) Engine=InnoDB;

create index `search_idx` on `search`(`email`);


drop table if exists `user_settings`;
create table if not exists `user_settings` (
   `username` char(64) not null unique,
   `pagelen` int default 20,
   `theme` char(8) default 'default',
   `lang` char(2) default 'en'
);

create index `user_settings_idx` on `user_settings`(`username`);


drop table if exists `user`;
create table if not exists `user` (
   `uid` int unsigned not null primary key,
   `username` char(64) not null unique,
   `realname` char(64) default null,
   `password` char(128) default null,
   `domain` char(64) default null,
   `dn` char(255) default '*',
   `isadmin` tinyint default 0
) Engine=InnoDB;

insert into `user` (`uid`, `username`, `realname`, `password`, `isadmin`, `domain`) values (0, 'admin', 'built-in piler admin', '$1$PItc7d$zsUgON3JRrbdGS11t9JQW1', 1, 'local');

drop table if exists `email`;
create table if not exists `email` (
   `uid` int unsigned not null,
   `email` char(128) not null primary key
) ENGINE=InnoDB;

insert into `email` (`uid`, `email`) values(0, 'admin@local');


create table if not exists `email_groups` (
   `uid` int unsigned not null,
   `gid` int unsigned not null,
   unique key `uid` (`uid`,`gid`),
   key `email_groups_idx` (`uid`,`gid`)
) ENGINE=InnoDB;


create table if not exists `group` (
   `id` bigint unsigned not null auto_increment primary key,
   `groupname` char(255) not null unique
) ENGINE=InnoDB;


create table if not exists `group_user` (
   `id` bigint unsigned not null,
   `uid` int unsigned not null,
   key `group_user_idx` (`id`),
   key `group_user_idx2` (`uid`)
) ENGINE=InnoDB;


create table if not exists `group_email` (
   `id` bigint unsigned not null,
   `email` char(128) not null,
   key `group_email_idx` (`id`)
) ENGINE=InnoDB;


create table if not exists `folder` (
   `id` int not null auto_increment,
   `parent_id` int default 0,
   `name` char(64) not null unique,
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
   unique(uid, name)
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


create table if not exists `remote` (
   `remotedomain` char(64) not null primary key,
   `remotehost` char(64) not null,
   `basedn` char(255) not null,
   `binddn` char(255) not null,
   `sitedescription` char(64) default null
) ENGINE=InnoDB;


drop table if exists `domain`;
create table if not exists `domain` (
   `domain` char(64) not null primary key,
   `mapped` char(64) not null
) ENGINE=InnoDB;

insert into `domain` (`domain`, `mapped`) values('local', 'local');


drop table if exists `audit`;
create table if not exists `audit` (
   `id` bigint unsigned not null auto_increment,
   `ts` int not null,
   `email` char(128) not null,
   `action` int not null,
   `ipaddr` char(15) not null,
   `meta_id` bigint unsigned not null,
   `description` char(255) default null,
   `vcode` char(64) default null,
   primary key (`id`)
) ENGINE=InnoDB;

create index `audit_idx` on `audit`(`email`);
create index `audit_idx2` on `audit`(`action`);
create index `audit_idx3` on `audit`(`ipaddr`);
create index `audit_idx4` on `audit`(`ts`);

