alter table timestamp change column hash_value hash_value varchar(128) default null;

alter table sph_index change column `to` `to` blob(8192) default null;
alter table sph_index change column fromdomain fromdomain tinyblob default null;
alter table sph_index change column todomain todomain blob(512) default null;


alter table metadata change column subject `subject` blob(512) default null;


SELECT Count(*) INTO @exists FROM information_schema.tables WHERE table_schema = 'piler' AND table_type = 'BASE TABLE' AND table_name = 'group';
SET @query = If(@exists>0, 'RENAME TABLE `group` TO usergroup', 'SELECT 1 from dual');
PREPARE stmt FROM @query;
EXECUTE stmt;
