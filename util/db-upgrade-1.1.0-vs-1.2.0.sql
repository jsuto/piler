-- 2015.03.10

alter table archiving_rule add column `body` varchar(128) default null;
alter table retention_rule add column `body` varchar(128) default null;

create unique index `entry` on archiving_rule (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);
create unique index `entry` on retention_rule (`domain`,`from`,`to`,`subject`,`body`,`_size`,`size`,`attachment_name`,`attachment_type`,`_attachment_size`,`attachment_size`,`spam`);

-- 2015.03.29

create index metadata_idx10 on metadata(`from`);

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


