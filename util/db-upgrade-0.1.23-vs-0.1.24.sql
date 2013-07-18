alter table `domain` add column `ldap_id` int default 0;

alter table `audit` add column `domain` varchar(128) not null,

create index `audit_idx5` on `audit`(`domain`);

create table if not exists `ldap` (
   `id` int not null auto_increment primary key,
   `description` varchar(255) not null,
   `ldap_type` varchar(255) not null,
   `ldap_host` varchar(255) not null,
   `ldap_base_dn` varchar(255) not null,
   `ldap_bind_dn` varchar(255) not null,
   `ldap_bind_pw` varchar(255) not null
) Engine=InnoDB;

create table if not exists `customer_settings` (
   `domain` varchar(255) not null unique,
   `branding_text` varchar(255) default null,
   `branding_url` varchar(255) default null,
   `branding_logo` varchar(255) default null,
   `support_link` varchar(255) default null,
   `colour` varchar(255) default null
) Engine=InnoDB;                                                                                                                                                                                                                                                                                                               


