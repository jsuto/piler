alter table `sph_index` add column `folder` int default 0;

create table if not exists `folder` (
   `id` int not null auto_increment,
   `parent_id` int default 0,
   `name` char(64) not null unique,
   primary key (`id`)
) Engine=InnoDB;


create table if not exists `note` (
   `id` bigint unsigned not null,
   `uid` int not null,
   `note` text default null,
   unique(`id`, `uid`),
   key (`id`)
) ENGINE=InnoDB;

