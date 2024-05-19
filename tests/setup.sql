use piler;

insert into domain (domain, mapped) values("fictive.com","fictive.com"),("address.com","address.com"),("acts.hu","acts.hu"),("gtsce.com","gtsce.com"),("datanet.hu","datanet.hu"),("gtsdatanet.hu","gtsdatanet.hu"),("gts.hu","gts.hu"),("aaa.fu","aaa.fu");

insert into archiving_rule (subject) values ("Android táblagép");
insert into archiving_rule (`from`) values ("@gmail.com");
insert into archiving_rule (`from`,attachment_type, _attachment_size, attachment_size) values ("finderis.co.ua", "image", ">", 100000);
insert into archiving_rule (`to`) values ("undisclosed-recipients");
insert into import (`type`, username, password, server) values ("imap-ssl", "sanyi@aaa.fu", "abcde123", "imap.aaa.fu");

update user set password="$6$GKL00T$8jqoFOe3PyAbOCLwKB7JwndwC.IinHrZRkdoQDZUc8vybZ88sA2qomlz5JceNif8fFpkGzZ03ilvQa7tqQx0v1";

insert into user (uid, username, realname, password, domain, dn, isadmin) values(2, "dataofficer", "Data officer", "$6$rX285LfP$ZxhlacbzKuCcqkaizzBu8SAiYb6.f8K4Us08nUHwSpWMQkNhw4o2rmfKXoTfaM4rnBHUYVK1N4IfBsqN8CAtS/", "local", "*", 4);
insert into email (uid, email) values(2, "do@local");

insert into legal_hold (email) values('aaa@aaa.fu'), ('bbb@bbb.fu');

insert into usergroup (groupname) values('group1');
insert into group_user (id, email) values(1, 'bela@aaa.fu'), (1, 'sanyi@aaa.fu');
insert into group_email (id, email) values(1, 'hirlevel@gruppi.com');

insert into autosearch (query) values('subject:slazenger');

insert into search(email, ts, term) values('auditor@local', 1716110897, 'page=0&sort=1&order=date&type=search&search=a%3Apdf&searchtype=expert&ref=&folders=&extra_folders=');
insert into search(email, ts, term) values('auditor@local', 1716111470, 'page=0&sort=1&order=date&type=search&search=subject%3Aslazenger&searchtype=expert&ref=&folders=&extra_folders=');
