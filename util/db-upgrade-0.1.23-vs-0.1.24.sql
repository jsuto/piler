alter table `domain` add column `ldap_id` int default 0;

alter table `audit` add column `domain` varchar(128) not null;

create index `audit_idx5` on `audit`(`domain`);

alter table archiving_rule add column domain varchar(255) default null;
alter table retention_rule add column domain varchar(255) default null;

alter table retention_rule drop index `from`; 
create unique index `entry` on retention_rule (`domain`,`from`,`to`,`subject`,`_size`,`size`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

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
   `colour` varchar(255) default null
) Engine=InnoDB;

create table if not exists `online` (
   `username` varchar(255) not null,
   `ts` int default 0,
   `last_activity` int default 0,
   `ipaddr` varchar(255) default null,
   unique(`username`,`ipaddr`)
) Engine=InnoDB;

