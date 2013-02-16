create table if not exists `group` (
   `id` bigint unsigned not null auto_increment primary key,
   `groupname` char(255) not null unique
) ENGINE=InnoDB;


create table if not exists `group_email` (
   `id` bigint unsigned not null,
   `email` char(128) not null,
   key `group_email_idx` (`id`)
) ENGINE=InnoDB;

create table if not exists `group_user` (
   `id` bigint unsigned not null,
   `uid` int unsigned not null,
   key `group_user_idx` (`id`),
   key `group_user_idx2` (`uid`)
) ENGINE=InnoDB;



