
alter table `user_settings` add column `ga_enabled` int default 0;
alter table `user_settings` add column `ga_secret` varchar(255) default null;

create index metadata_idx9 on metadata(`sent`);

alter table archiving_rule add column `attachment_name` varchar(128) default null;
alter table retention_rule add column `attachment_name` varchar(128) default null;

alter table archiving_rule drop index `from`; 
create unique index `entry` on archiving_rule (`domain`,`from`,`to`,`subject`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

alter table retention_rule drop index `entry`;
create unique index `entry` on retention_rule (`domain`,`from`,`to`,`subject`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

