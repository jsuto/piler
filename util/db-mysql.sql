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
  `piler_id` char(36) not null,
  `header_id` char(16) default null,
  `body_id` char(16) default null,
  primary key (`id`)
) Engine=InnoDB;

drop table if exists `metadata`;
create table `metadata` (
  `id` bigint unsigned not null auto_increment,
  `from` char(255) not null,
  `to` char(255) default null,
  `subject` text(512) default null,
  `arrived` int not null,
  `sent` int not null,
  `deleted` int default 0,
  `size` int default 0,
  `hlen` int default 0,
  `piler_id` char(36) not null,
  `message_id` char(128) character set 'latin1' not null,
  `bodydigest` char(64) not null,
  primary key (`id`), unique(`to`,`message_id`)
) Engine=InnoDB;

create index metadata_idx on metadata(`piler_id`);
create index metadata_idx2 on metadata(`message_id`); 
create index metadata_idx3 on metadata(`bodydigest`); 

drop table if exists `counter`;
create table if not exists `counter` (
   `rcvd` bigint unsigned default 0,
   `virus` bigint unsigned default 0,
   `duplicate` bigint unsigned default 0
) Engine=InnoDB;

insert into `counter` values(0, 0, 0);

