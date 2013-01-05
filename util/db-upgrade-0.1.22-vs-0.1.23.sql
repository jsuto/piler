
create table if not exists `domain_user` (
   `domain` char(64) not null,
   `uid` int unsigned not null,
   key `domain_user_idx` (`domain`),
   key `domain_user_idx2` (`uid`)
) ENGINE=InnoDB;

