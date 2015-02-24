%define name piler
%define version 1.1.1
%define release 1

Summary:        an email archiving application
Name:           %{name}
Version:        %{version}
Release:        %{release}
License:        gpl
Group:          Networking/Mail
Source0:        %{name}-%{version}.tar.gz
URL:            http://www.mailpiler.org/
Buildroot:      /tmp/aa
BuildRequires:  openssl-devel, tcp_wrappers-devel, poppler-utils, libzip-devel, catdoc, mysql-devel, tnef, unrtf, tre-devel
Requires:       mysql, openssl, tcp_wrappers, libzip, poppler-utils, catdoc, tnef, unrtf, tre, httpd, php, php-mysql
Provides:       libpiler.so()(64bit)

%description
piler is an email archiving application.

%prep
%setup

%build
./configure --prefix=/usr --libdir=/usr/lib64 --sysconfdir=/etc --localstatedir=/var --enable-starttls --enable-tcpwrappers --with-database=mysql
make clean all

%install
mkdir -p $RPM_BUILD_ROOT/etc/init.d
make install DESTDIR=$RPM_BUILD_ROOT

# Gotta also copy over a few other things
mkdir -p $RPM_BUILD_ROOT/var/www
cp -ar %{_topdir}/BUILD/%{name}-%{version}/webui $RPM_BUILD_ROOT/var/piler/www

mkdir -p $RPM_BUILD_ROOT/usr/share/piler
cp -r %{_topdir}/BUILD/%{name}-%{version}/contrib $RPM_BUILD_ROOT/usr/share/piler

# Fix the base piler.conf file
cp -f %{_topdir}/BUILD/%{name}-%{version}/etc/example.conf $RPM_BUILD_ROOT/etc/piler.conf.dist

%files
%defattr(-,root,root)
%dir /var/piler
%dir /var/piler/tmp
%dir /var/piler/sphinx
%dir /var/piler/store
%dir /var/piler/stat
%dir /var/run/piler
%attr(0655,piler,piler) /usr/bin/pileraget
%attr(0655,piler,piler) /usr/bin/pilerexport
%attr(0655,piler,piler) /usr/bin/pilerget
%attr(0655,piler,piler) /usr/bin/pilerpurge
%attr(0655,piler,piler) /usr/bin/pilerimport
%attr(0655,piler,piler) /usr/bin/reindex
/etc/init.d/rc.piler
/etc/init.d/rc.pilergetd
/etc/init.d/rc.searchd
%attr(0640,root,piler) /etc/piler.conf
/etc/sphinx.conf.dist
/usr/sbin/piler
/usr/sbin/pilergetd
/usr/sbin/pilerconf
/usr/lib64/libpiler.a
/usr/lib64/libpiler.so
/usr/lib64/libpiler.so.0
/usr/lib64/libpiler.so.0.1.1
/usr/libexec/piler/daily-report.php
/usr/libexec/piler/generate_stats.php
/usr/libexec/piler/gmail-imap-import.php
/usr/libexec/piler/indexer.delta.sh
/usr/libexec/piler/indexer.main.sh
/usr/libexec/piler/import.sh
/usr/libexec/piler/postinstall.sh
/usr/libexec/piler/purge.sh
/usr/share/piler/db-mysql-root.sql.in
/usr/share/piler/db-mysql.sql


%pre
groupadd piler
useradd -g piler -s /bin/sh -d /var/piler piler
usermod -L piler
if [ -d /var/piler ]; then chmod 755 /var/piler; fi


%post
chown -R piler:piler /var/run/piler /var/piler
echo /usr/lib > /etc/ld.so.conf.d/piler.conf
ldconfig
echo this is the postinstall stuff...
echo run /usr/libexec/piler/postinstall.sh manually to configure piler

%postun
userdel piler
groupdel piler


%changelog
* Tue Feb 24 2015 Janos Suto
  - 1.1.1 release of piler

* Fri Nov  1 2013 Janos Suto
  - Fixed a bug causing issues when reading the retention|archiving_rules tables

* Fri Oct 25 2013 Janos Suto
  - First release of the rpm package based on build 846


