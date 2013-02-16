
create table if not exists `domain_user` (
   `domain` char(64) not null,
   `uid` int unsigned not null,
   key `domain_user_idx` (`domain`),
   key `domain_user_idx2` (`uid`)
) ENGINE=InnoDB;


CREATE TABLE IF NOT EXISTS `counter_stats` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `date` int(11) NOT NULL,
  `email` varchar(255) NOT NULL,
  `domain` varchar(255) NOT NULL,
  `sent` int(11) NOT NULL,
  `recd` int(11) NOT NULL,
  `sentsize` int(11) NOT NULL,
  `recdsize` int(11) NOT NULL,
  PRIMARY KEY (`id`),
  KEY `date` (`date`),
  KEY `email` (`email`),
  KEY `domain` (`domain`)
) ENGINE=InnoDB;

alter table metadata drop index message_id;

