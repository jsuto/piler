create table if not exists `group` (
   `id` bigint unsigned not null auto_increment primary key,
   `groupname` char(255) not null unique
) ENGINE=InnoDB;


create table if not exists `group_email` (
   `id` bigint unsigned not null,
   `email` char(128) not null,
   key `group_email_idx` (`id`)
) ENGINE=InnoDB;

alter table `user` add column `gid` int default 0;


