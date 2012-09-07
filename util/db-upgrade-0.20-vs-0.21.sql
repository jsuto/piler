alter table `sph_index` add column `folder` int default 0;

drop table if exists `tag`;
create table if not exists `tag` (
   `_id` bigint unsigned auto_increment not null,
   `id` bigint not null,
   `uid` int not null,
   `tag` char(255) default null,
   unique(`id`, `uid`),
   key (`_id`)
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


create table if not exists `note` (
   `_id` bigint unsigned auto_increment not null,
   `id` bigint unsigned not null,
   `uid` int not null,
   `note` text default null,
   unique(`id`, `uid`),
   key (`_id`)
) ENGINE=InnoDB;


