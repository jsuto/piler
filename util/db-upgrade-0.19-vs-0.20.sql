alter table `user` change column `password` `password` char(128) default null;
alter table `remote` change column `basedn` `basedn` char(255) not null;
alter table `remote` change column `binddn` `binddn` char(255) not null;

