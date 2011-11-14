SHELL = /bin/sh

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
DEFS =  -D_GNU_SOURCE -DHAVE_ANTIVIRUS -DHAVE_CLAMD -DHAVE_TRE -DNEED_MYSQL -DNEED_SQLITE3
INCDIR = -I. -I..  -I/usr/local/mysql/include -fPIC -g -static-libgcc -fno-omit-frame-pointer  -m32 -fPIC -g -static-libgcc -fno-omit-frame-pointer -fno-strict-aliasing -DMY_PTHREAD_FASTMUTEX=1
LIBDIR = -L.  
LIBS = -lz -lm -ldl -lcrypto -ltre  -lsqlite3 -lpthread -L/usr/local/mysql/lib -lmysqlclient_r -lpthread -lm -lrt -ldl -lguide
OBJS = dirs.o misc.o counters.o cfg.o sig.o decoder.o list.o boundary.o parser.o parser_utils.o session.o message.o digest.o store.o tai.o  avir.o clamd.o
MYSQL_OBJS = 
RUNNING_USER = piler
RUNNING_GROUP = `id -gn $(RUNNING_USER)`

PILER_VERSION=0
PILER_REVISION=1
PILER_RELEASE=1
LIBPILER_VERSION=$(PILER_VERSION).$(PILER_REVISION).$(PILER_RELEASE)

MAKE = `which make`

INSTALL = /bin/ginstall -c

all: libpiler.a piler pilerconf test
install: install-piler


piler: piler.c libpiler.a
	$(CC) $(CFLAGS) $(INCDIR) $(DEFS) -o $@ piler.c -lpiler $(LIBS) $(LDAP_LIBS) $(LIBDIR)  

libpiler.a: $(OBJS) $(MYSQL_OBJS)
	ar cr libpiler.a $(OBJS) $(MYSQL_OBJS)
	ranlib libpiler.a
	$(CC) -shared -Wl -o libpiler.so.$(LIBPILER_VERSION) $(OBJS) $(MYSQL_OBJS) $(LIBS) $(LDAP_LIBS) 
	ln -sf libpiler.so.$(LIBPILER_VERSION) libpiler.so
	ln -sf libpiler.so.$(LIBPILER_VERSION) libpiler.so.$(PILER_VERSION)


pilerconf: pilerconf.c cfg.o misc.o tai.o
	$(CC) $(CFLAGS) $(INCDIR) $(DEFS) -o $@ $^ $(LIBS) $(LIBDIR)

test:
	$(CC) $(CFLAGS) $(INCDIR) $(DEFS) -o piletest $(srcdir)/test.c -lpiler $(LIBS) $(LDAP_LIBS) $(LIBDIR) 

%.o: $(srcdir)/%.c
	$(CC) $(CFLAGS) -fPIC $(INCDIR) $(DEFS) -c $< -o $@


install-piler:
	$(INSTALL) -d $(DESTDIR)$(bindir)
	$(INSTALL) -d $(DESTDIR)$(sbindir)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) -m 0644 libpiler.a $(DESTDIR)$(libdir)
	$(INSTALL) -m 0755 libpiler.so.$(LIBPILER_VERSION) $(DESTDIR)$(libdir)
	(cd $(DESTDIR)$(libdir) && ln -sf libpiler.so.$(LIBPILER_VERSION) libpiler.so)
	(cd $(DESTDIR)$(libdir) && ln -sf libpiler.so.$(LIBPILER_VERSION) libpiler.so.$(PILER_VERSION))

	$(INSTALL) -d $(DESTDIR)$(libexecdir)/piler

	$(INSTALL) -d $(DESTDIR)$(datarootdir)/piler

	$(INSTALL) -m 0755 piler $(DESTDIR)$(sbindir)
	$(INSTALL) -m 0755 pilerconf $(DESTDIR)$(sbindir)

clean:
	rm -f *.o *.a libpiler.so* piler pilerconf piletest

distclean: clean
	rm -f Makefile

