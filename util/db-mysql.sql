drop table if exists `sph_counter`;
create table `sph_counter` (
  `counter_id` int not null,
  `max_doc_id` int not null,
  primary key (`counter_id`)
);

drop table if exists `sph_index`;
create table `sph_index` (
  `id` int not null auto_increment,
  `from` char(255) default null,
  `to` text(512) default null,
  `subject` text(512) default null,
  `arrived` int not null,
  `sent` int not null,
  `body` text,
  `size` int default '0',
  `attachments` int default 0,
  `piler_id` char(36) not null,
  primary key (`id`)
) Engine=InnoDB;


drop table if exists `metadata`;
create table `metadata` (
  `id` bigint unsigned not null auto_increment,
  `from` char(255) not null,
  `to` text(2048) character set 'latin1' not null,
  `subject` text(512) default null,
  `arrived` int not null,
  `sent` int not null,
  `deleted` int default 0,
  `size` int default 0,
  `hlen` int default 0,
  `attachments` int default 0,
  `piler_id` char(36) not null,
  `message_id` char(128) character set 'latin1' not null,
  `digest` char(64) not null,
  `bodydigest` char(64) not null,
  primary key (`id`), unique(`message_id`)
) Engine=InnoDB;

create index metadata_idx on metadata(`piler_id`);
create index metadata_idx2 on metadata(`message_id`); 
create index metadata_idx3 on metadata(`bodydigest`); 

drop table if exists `attachment`;
create table `attachment` (
   `id` bigint unsigned not null auto_increment,
   `piler_id` char(36) not null,
   `attachment_id` int not null,
   `name` char(64) default null,
   `type` char(72) default null,
   `sig` char(64) not null,
   `size` int default 0,
   `ptr` int default 0,
   primary key (`id`)
) Engine=InnoDB;

create index `attachment_idx` on `attachment`(`piler_id`);
create index `attachment_idx2` on `attachment`(`sig`);

drop table if exists `archiving_rule`;
create table `archiving_rule` (
   `id` bigint unsigned not null auto_increment,
   `from` char(128) default null,
   `to` char(255) default null,
   `subject` char(255) default null,
   `_size` char(2) default null,
   `size` int default 0,
   `attachment_type` char(128) default null,
   `_attachment_size` char(2) default null,
   `attachment_size` int default 0,
   primary key (`id`)
) ENGINE=InnoDB;

drop table if exists `counter`;
create table if not exists `counter` (
   `rcvd` bigint unsigned default 0,
   `virus` bigint unsigned default 0,
   `duplicate` bigint unsigned default 0
) Engine=InnoDB;

insert into `counter` values(0, 0, 0);

drop table if exists `search`;
create table `search` (
   `email` char(128) not null,
   `ts` int default 0,
   `term` text(512) not null
) Engine=InnoDB;

create index `search_idx` on `search`(`email`);


