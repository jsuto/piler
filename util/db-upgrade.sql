alter table timestamp change column hash_value hash_value varchar(128) default null;

alter table sph_index change column `to` `to` blob(8192) default null;
alter table sph_index change column fromdomain fromdomain tinyblob default null;
alter table sph_index change column todomain todomain blob(512) default null;


alter table metadata change column subject `subject` blob(512) default null;
