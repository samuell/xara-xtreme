#!/usr/bin/perl
# getdeps.pl

use strict;
use File::Basename;
use Getopt::Long;

sub usage
{
    print STDERR <<EOF;

 getdeps.pl - get camelot dependencies

   Usage:

 getdeps.pl [options] file...

 file ... should be one or more Po files from the relevant deps directory
          so for instance '*/.deps/*.Po' means everything (add a build
          directory if relevant).

 Options:
   -t target           - Print files that "target" depends on
   -i includedfile     - Print targets that depend on includedfile
   -e                  - Explain why (in context of -t or -n)
                         This may take some time
   -n                  - Just print the totals
   --help              - display this message

EOF

my $error = shift @_;

    if ($error)
    {
        die ($error);
    }
    else
    {
        exit (0);
    }

}

# the non-recursive, duplicate-ridden hash
my %depend;

# the forward hash
my %targetdependencies;
# the reverse hash
my %includedependencies;

my $target="";
my $includedfile="";
my $explain=0;
my $help=0;
my $verbose=0;
my $total=0;


GetOptions( "target|t=s" => \$target,
            "includedfile|i=s" => \$includedfile,
            "explain|f!" => \$explain,
            "total|n!" => \$total,
            "verbose|v+" => \$verbose,
            "help!" => \$help ) || usage ("Bad option");

usage() if ($help);

sub dotarget
{
    my $n = shift @_;
    my $a;
    if (defined($a=$targetdependencies{$n}))
    {
	if ($total)
	{
	    my @a=split(" ",$a);
	    printf "%d $n\n",$#a+1;
	}
	else
	{
	    printf "$n: $a\n";
	}

    }
    else
    {
	if ($total)
	{
	    print "0 $n\n";
	}
	else
	{
	    print "$n: Not found!\n";
	}
    }
}

sub doinclude
{
    my $n = shift @_;
    my $a;
    if (defined($a=$includedependencies{$n}))
    {
	if ($total)
	{
	    my @a=split(" ",$a);
	    printf "%d $n\n",$#a+1;
	}
	else
	{
	    printf "$n: $a\n";
	}

    }
    else
    {
	if ($total)
	{
	    print "0 $n\n";
	}
	else
	{
	    print "$n: Not found!\n";
	}
    }
}

sub remembertarget
{
    my $t = shift @_;
    # ignore blank ones
    next if ($t=~/^\s*$/);
    
    # ignore system dependencies
    next if ($t=~/^\//);
    
    # ignore targets in the wxXtra directory
    next if ($t=~/wxXtra\//);

    $t=basename($t);
    $targetdependencies{$t}.="";
}

sub getdepend
{
    my $f;
    my $l;
    $f = shift @_;
    my $d;
    my %deps;

    foreach $d (split (/\s+/,$depend{$f}))
    {
	# ignore blank ones
	next if ($d=~/^\s*$/);

	# ignore system dependencies
	next if ($d=~/^\//);

	# ignore stuff in the wxXtra directory
	next if ($d=~/wxXtra\//);

	$d=basename($d);

	$deps{$d}=1;
	# add the dependencies (if any) this has
	my @recurse;
	@recurse=getdepend($d,$l+1);
	my $k;
	foreach $k (@recurse)
	{
	    $deps{$k}=1;
	}
    }
    return sort(keys %deps);
}



sub process
{
    my $fn = shift @_;
    open (DEPFILE, $fn) || die ("Can't open deps file $fn - maybe you need to build first: $!");

    my $line;
    while (defined($line=<DEPFILE>))
    {
	chomp $line;
	# Handle continuation characters
	if ($line =~ s/\\\s*$//)
	{
	    $line .= " ".<DEPFILE>;
	    redo unless eof(DEPFILE);
	}
	
	$line.=" ";

	# skip blank lines
	next if ($line=~/^\s*$/);
	
	# skip lines without a colon
	next unless ($line=~/:/);
	
	# split into before and after colon
	my $depends;
	my $dependson;
	($depends,$dependson)=split(/\s*:\s*/ , $line);

	# skip non-header files. The fact object files depend on the cpp is
	# uninteresting
	$dependson = " ".$dependson." ";
	$dependson =~ s/\s+\S+\.(cpp|c)\s+/ /g;
	$dependson =~ s/\s+/ /g;
	$dependson =~ s/^\s+//g;

	my $dep;
	foreach $dep (split(/\s+/, $depends))
	{
	    $dep=~s/\s+//g;
	    next if ($dep=~/^\s*$/);

	    $depend{$dep}=$depend{$dep}.$dependson;
	    # pretend everything depends on camtypes (it ought to in the main)
	    $depend{$dep}.="camtypes.h.gch " unless ($dep eq "camtypes.h.gch");

	    remembertarget($dep);
	}
    }
    
    close(DEPFILE);
}


# If no files specified, fake stdin as a file
push (@ARGV,"-") if ($#ARGV<0);

# Now iterate through each of the files specified on the command line

my $ifn;

# Read in all the dep files
foreach $ifn (@ARGV)
{
    if ($ifn eq "-")
    {
        $ifn="<&0";
    }

    # process the file
    process ($ifn, ($ifn eq "<&0")?"<STDIN>":$ifn);
}

# Now build the maps;
my $t;
foreach $t (sort keys %targetdependencies)
{
    my @depends=getdepend($t);
    # these were blank before
    $targetdependencies{$t}=join(" ",@depends);
    my $d;
    foreach $d (@depends)
    {
	if (defined($includedependencies{$d}))
	{
	    $includedependencies{$d}.=" ".$t;
	}
	else
	{
	    $includedependencies{$d}=$t;
	}
    }
}

# Now sort the include dependencies
my $d;
foreach $d (sort keys %includedependencies)
{
    $includedependencies{$d}=join(" ",sort split(" ",$includedependencies{$d}));
}

if ($target ne "")
{
    if ($target eq "all")
    {
	foreach $t (sort keys %targetdependencies)
	{
	    dotarget($t);
	}
    }
    else
    {
	dotarget($target);
    }
}

if ($includedfile ne "")
{
    if ($includedfile eq "all")
    {
	foreach $d (sort keys %includedependencies)
	{
	    doinclude($d);
	}
    }
    else
    {
	doinclude($includedfile);
    }
}


