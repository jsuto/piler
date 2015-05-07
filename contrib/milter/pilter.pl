#!/usr/bin/perl

use strict;

use Carp qw(verbose);
use Sendmail::PMilter qw(:all);
use Data::Dumper;
use Sys::Syslog;
use Sys::Syslog qw(:DEFAULT setlogsock);
use Time::HiRes qw(gettimeofday);

my $savedir = "/var/piler/imap";
my $username = 'piler';
my $conn = 'inet:33333@127.0.0.1';

my $miltername = 'pilter';
my $priority = "mail|info";
my $fname = '';
my $msglen = 0;
my $newdir = "new";
my $curdir = "cur";
my $newname;
my $curname;
my $messageid;


chdir $savedir || die("cannot chdir to $savedir");

my %cbs;
for my $cb (qw(close connect helo abort envfrom envrcpt header eoh body eom)) {
	$cbs{$cb} = sub {
		my $ctx = shift;
                my ($seconds, $microseconds);

                ###if($cb eq "connect") {
                if($cb eq "envrcpt") {
                   ($seconds, $microseconds) = gettimeofday;
                   $fname = $seconds . "-" . $microseconds . "-" . $$ . "-" . &get_random_name;

                   ###syslog $priority, "fname=$fname";

                   if(! -d $newdir) { mkdir $newdir, 0700; }

                   $newname = $newdir . "/" . $fname;
                   $curname = $curdir . "/" . $fname;

                   $msglen = 0;
                   $messageid = '';

                   if(!open(F, ">$newname")) { return SMFIS_TEMPFAIL; }
                }

                elsif($cb eq "header") {
                   $msglen += length ( @_[0] . ": " . @_[1] . "\n" );
                   print F @_[0] . ": " . @_[1] . "\n";

                   if(@_[0] =~ /^message-id$/i) { $messageid = @_[1]; }
                }

                elsif($cb eq "eoh") {
                   print F "\n";
                }

                elsif($cb eq "body") {
                   $msglen += @_[1];
                   print F @_[0];
                }

                elsif($cb eq "eom") {
                   close F;

                   if(! -d $curdir) { mkdir $curdir, 0755; }

                   rename $newname, $curname;

                   syslog $priority, "message-id=$messageid, fname=$fname, size=$msglen";

                }

                #if ($cb =~ /^(connect|help|envfrom|envrcpt)$/) {
                #       print Dumper($ctx->{symbols})."\n";
                #}

		SMFIS_CONTINUE;
	}
}

openlog($miltername, 'pid', 'mail');

$< = $> = getpwnam $username;

syslog $priority, "$miltername starting";


my $milter = new Sendmail::PMilter;
$milter->setconn($conn);

$milter->register($miltername, \%cbs, SMFI_CURR_ACTS);

my $dispatcher = Sendmail::PMilter::prefork_dispatcher(
	max_children => 5,
	max_requests_per_child => 100,
);

$milter->set_dispatcher($dispatcher);

$milter->main();



sub get_random_name {
   my @chars = ('A'..'Z', 0..9);
   my $s = '';

   srand;

   $s .= $chars[rand @chars] for 1..8;

   return $s;
}


