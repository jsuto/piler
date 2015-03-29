-- 2015.03.10

alter table archiving_rule add column `body` varchar(128) default null;
alter table retention_rule add column `body` varchar(128) default null;

--drop index entry on archiving_rule;
--drop index entry on retention_rule;

create unique index `entry` on archiving_rule (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);
create unique index `entry` on retention_rule (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

create index metadata_idx10 on metadata(`from`);

