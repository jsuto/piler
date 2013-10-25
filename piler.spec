%define name piler
%define version 0.1.25
%define release 1

Summary:        an email archiving application
Name:           %{name}
Version:        %{version}
Release:        %{release}
License:        gpl
Group:          Networking/Mail
Source0:        master.tar.gz
URL:            http://www.mailpiler.org/
Buildroot:      /tmp/aa
BuildRequires:  openssl-devel, tcp_wrappers, poppler-utils, libzip-devel, catdoc, mysql-devel, tnef, unrtf, tre-devel
Requires:       mysql, openssl, tcp_wrappers, libzip, poppler-utils, catdoc, tnef, unrtf, tre

%description
piler is an email archiving application.

%prep
%setup

%build
./configure --localstatedir=/var --enable-starttls --enable-tcpwrappers --with-database=mariadb
make clean all

%install
##mkdir -p /tmp/aa/etc/init.d
##make install DESTDIR=/tmp/aa
mkdir -p /root/rpmbuild/BUILDROOT/piler-0.1.25-1.x86_64/etc/init.d
mkdir -p /root/rpmbuild/BUILDROOT/piler-0.1.25-1.x86_64/usr/local/lib
make install DESTDIR=/root/rpmbuild/BUILDROOT/piler-0.1.25-1.x86_64
cp /usr/local/lib/libmariadb* /root/rpmbuild/BUILDROOT/piler-0.1.25-1.x86_64/usr/local/lib


%files
%defattr(-,root,root)
%dir /var/piler
%dir /var/piler/tmp
%dir /var/piler/sphinx
%dir /var/piler/store
%dir /var/piler/stat
%dir /var/run/piler
%attr(0655,piler,piler) /usr/local/bin/pileraget
%attr(0655,piler,piler) /usr/local/bin/pilerexport
%attr(0655,piler,piler) /usr/local/bin/pilerget
%attr(0655,piler,piler) /usr/local/bin/pilerpurge
%attr(0655,piler,piler) /usr/local/bin/pilerimport
%attr(0655,piler,piler) /usr/local/bin/reindex
/etc/init.d/rc.piler
/etc/init.d/rc.pilergetd
/etc/init.d/rc.searchd
%attr(0640,root,piler) /usr/local/etc/piler.conf
/usr/local/etc/sphinx.conf.dist
/usr/local/sbin/piler
/usr/local/sbin/pilergetd
/usr/local/sbin/pilerconf
/usr/local/lib/libmariadbclient.a
/usr/local/lib/libmariadb.so
/usr/local/lib/libmariadb.so.1
/usr/local/lib/libpiler.a
/usr/local/lib/libpiler.so
/usr/local/lib/libpiler.so.0
/usr/local/lib/libpiler.so.0.1.1
/usr/local/libexec/piler/daily-report.php
/usr/local/libexec/piler/generate_stats.php
/usr/local/libexec/piler/gmail-imap-import.php
/usr/local/libexec/piler/indexer.delta.sh
/usr/local/libexec/piler/indexer.main.sh
/usr/local/libexec/piler/import.sh
/usr/local/libexec/piler/postinstall.sh
/usr/local/libexec/piler/purge.sh
/usr/local/share/piler/db-mysql-root.sql.in
/usr/local/share/piler/db-mysql.sql


%pre
groupadd piler
useradd -g piler -s /bin/sh -d /var/piler piler
usermod -L piler
if [ -d /var/piler ]; then chmod 755 /var/piler; fi


%post
chown -R piler:piler /var/run/piler /var/piler
echo /usr/local/lib > /etc/ld.so.conf.d/piler.conf
ldconfig
echo this is the postinstall stuff...
echo run /usr/local/libexec/piler/postinstall.sh manually to configure piler

%postun
userdel piler
groupdel piler


%changelog
* Fri Oct 25 2013 Janos Suto
  - First release of the rpm package based on build 846


