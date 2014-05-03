
alter table `user_settings` add column `ga_enabled` int default 0;
alter table `user_settings` add column `ga_secret` varchar(255) default null;

create index metadata_idx9 on metadata(`sent`);

alter table archiving_rule change column `from` `from` varchar(128) default null;
alter table archiving_rule change column `to` `to` varchar(128) default null;
alter table archiving_rule change column `subject` `subject` varchar(128) default null;
alter table retention_rule change column `from` `from` varchar(128) default null;
alter table retention_rule change column `to` `to` varchar(128) default null;
alter table retention_rule change column `subject` `subject` varchar(128) default null;

alter table archiving_rule add column `attachment_name` varchar(128) default null;
alter table retention_rule add column `attachment_name` varchar(128) default null;

alter table ldap add column ldap_mail_attr varchar(128) default null;
alter table ldap add column ldap_account_objectclass varchar(128) default null;
alter table ldap add column ldap_distributionlist_attr varchar(128) default null;
alter table ldap add column ldap_distributionlist_objectclass varchar(128) default null;

alter table google change column id id char(32) not null;
alter table google_imap change column id id char(32) not null primary key;


create table if not exists `autosearch` (
   `id` int not null auto_increment primary key,
   `query` varchar(512) not null
) Engine=InnoDB;


create unique index `entry` on archiving_rule (`domain`,`from`,`to`,`subject`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

create unique index `entry` on retention_rule (`domain`,`from`,`to`,`subject`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

