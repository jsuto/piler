alter table `sph_index` add column `folder` int default 0;

drop table if exists `folder`;
create table if not exists `folder` (
   `id` int not null auto_increment,
   `parent_id` int default 0,
   `name` char(64) not null unique,
   primary key (`id`)
) Engine=InnoDB;


