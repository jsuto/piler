
alter table `user_settings` add column `ga_enabled` int default 0;
alter table `user_settings` add column `ga_secret` varchar(255) default null;

create index metadata_idx9 on metadata(`sent`);


