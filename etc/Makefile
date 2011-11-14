SHELL = /bin/bash

prefix = /usr/local
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
includedir = ${prefix}/include
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec
srcdir = .
sysconfdir = ${prefix}/etc
mandir = ${datarootdir}/man
datarootdir = ${prefix}/share
localstatedir = /var

CC = gcc
CFLAGS =  -O2 -Wall -g 
DEFS =  -DHAVE_MYSQL -D_GNU_SOURCE -DHAVE_ANTISPAM -DUSERS_IN_MYSQL -DHAVE_USERS -DNEED_MYSQL
INCDIR = -I. -I../.. -I../../src  -I/usr/include/mysql  -DBIG_JOINS=1  -fno-strict-aliasing   -DUNIV_LINUX -DUNIV_LINUX
LIBDIR = -L.   -L../../src
LIBS =   -lm -ldl    -rdynamic -L/usr/lib/mysql -lmysqlclient_r
LDAP_LIBS = 
CLAPF_USER = clapf
CLAPF_GROUP = `id -gn $(CLAPF_USER)`

INSTALL = /usr/bin/install -c

all:
	sed -e 's%pidfile=.*%pidfile=$(localstatedir)/run/clapf/clapf.pid%g' \
	-e 's%sqlite3=.*%sqlite3=$(localstatedir)/lib/clapf/data/tokens.sdb%g' \
	-e 's%workdir=.*%workdir=$(localstatedir)/spool/clapf/tmp%g' \
	-e 's%queuedir=.*%queuedir=$(localstatedir)/lib/clapf/queue%g' \
	-e 's%mydbfile=.*%mydbfile=$(localstatedir)/lib/clapf/tokens.mydb%g' < $(srcdir)/example.conf > $(srcdir)/clapf.conf

	sed -e 's%LOCALSTATEDIR%$(localstatedir)%g' \
	-e 's%LIBEXECDIR%$(libexecdir)%g' \
	-e 's%SYSCONFDIR%$(sysconfdir)%g' \
	-e 's%SBINDIR%$(sbindir)%g' \
	-e 's%DATAROOTDIR%$(datarootdir)%g' < $(srcdir)/cron.jobs.in > $(srcdir)/cron.jobs

install:
	if [ ! -f "$(DESTDIR)$(sysconfdir)/clapf.conf" ]; then $(INSTALL) -m 0640 $(srcdir)/clapf.conf $(DESTDIR)$(sysconfdir)/clapf.conf; chgrp $(CLAPF_GROUP) $(DESTDIR)$(sysconfdir)/clapf.conf; fi

clean:
	rm -f clapf.conf cron.jobs

distclean: clean
	rm -f Makefile
