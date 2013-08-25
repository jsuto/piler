alter table `domain` add column `ldap_id` int default 0;

alter table `audit` add column `domain` varchar(128) not null;

create index `audit_idx5` on `audit`(`domain`);

alter table archiving_rule add column domain varchar(255) default null;
alter table retention_rule add column domain varchar(255) default null;

alter table retention_rule drop index `from`; 
create unique index `entry` on retention_rule (`domain`,`from`,`to`,`subject`,`_size`,`size`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

drop view if exists `v_messages`;
create view `v_messages` AS select `metadata`.`id` AS `id`,`metadata`.`piler_id` AS `piler_id`,`metadata`.`from` AS `from`,`metadata`.`fromdomain` AS `fromdomain`,`rcpt`.`to` AS `to`,`rcpt`.`todomain` AS `todomain`,`metadata`.`subject` AS `subject`, `metadata`.`size` AS `size`, `metadata`.`direction` AS `direction`, `metadata`.`sent` AS `sent`, `metadata`.`retained` AS `retained`, `metadata`.`arrived` AS `arrived`, `metadata`.`digest` AS `digest`, `metadata`.`bodydigest` AS `bodydigest` from (`metadata` join `rcpt`) where (`metadata`.`id` = `rcpt`.`id`);

update user_settings set theme='mobile' where theme='orig';

create table if not exists `ldap` (
   `id` int not null auto_increment primary key,
   `description` varchar(255) not null,
   `ldap_type` varchar(255) not null,
   `ldap_host` varchar(255) not null,
   `ldap_base_dn` varchar(255) not null,
   `ldap_bind_dn` varchar(255) not null,
   `ldap_bind_pw` varchar(255) not null,
   `ldap_auditor_member_dn` varchar(255) default null
) Engine=InnoDB;

create table if not exists `customer_settings` (
   `id` int not null auto_increment primary key,
   `domain` varchar(255) not null unique,
   `branding_text` varchar(255) default null,
   `branding_url` varchar(255) default null,
   `branding_logo` varchar(255) default null,
   `support_link` varchar(255) default null,
   `background_colour` varchar(255) default null,
   `text_colour` varchar(255) default null
) Engine=InnoDB;

create table if not exists `online` (
   `username` varchar(255) not null,
   `ts` int default 0,
   `last_activity` int default 0,
   `ipaddr` varchar(255) default null,
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

