#!/usr/bin/env perl
#
use strict;
use warnings;
use IO::File;
use File::Basename;
use Getopt::Long;

sub help {
    print basename($0), "\nhelp:\n";
    print "--ifile = file_name\n";
    exit 1;
}

my $opt_fname;
my $opt_help;
GetOptions('ifile=s' => \$opt_fname, 'help|?' => \$opt_help) or help;
help if defined $opt_help;
print "No file name passed\n" and exit 1 if not defined $opt_fname;

my $fh = new IO::File;
my $fho = new IO::File;

$fh->open("<$opt_fname");
$fho->open(">all_mnecs.txt");
while(<$fh>){
    if(/X\(([\w\.\s]+)\)/){
        $fho->printf("%s\n", $1);
    }
}

$fh->close;
$fho->close;
