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
   -w                  - warn if camtypes.h is not the first included file
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

# Filenames
my %fnhash;

# Directories
my @dirs=( "GDraw", "Kernel", "PreComp", "Kernel", "tools", "wxOil", "wxXtra" );

# the hash of direct includes
my %di;
# and its reverse
my %diby;

# counts of total includes
my %cincluded;
my %ctarget;

my $target="";
my $includedfile="";
my $explain=0;
my $help=0;
my $verbose=0;
my $total=0;
my $warning=0;

GetOptions( "target|t=s" => \$target,
            "includedfile|i=s" => \$includedfile,
            "explain|f!" => \$explain,
            "total|n!" => \$total,
            "warning|w!" => \$warning,
            "verbose|v+" => \$verbose,
            "help!" => \$help ) || usage ("Bad option");

usage() if ($help);

sub counttarget
{
    my $f=shift @_;
    my $l=shift @_;
    my $c=shift @_;
    return $ctarget{$f} if (defined($ctarget{$f}));

    my $children=($di{$f} eq "")?0:1;
    my $count=1;

    if ($children)
    {
	my $t;
	foreach $t (sort split(/\s+/,$di{$f}))
	{
	    # detect loops
	    if ($c!~/ $t /)
	    {
		$count+=counttarget($t, $l+1, $c." ".$t." ");
	    }
	}
    }
    $ctarget{$f}=$count;
    return $count;
}

sub countincluded
{
    my $f=shift @_;
    my $l=shift @_;
    my $c=shift @_;
    return $cincluded{$f} if (defined($cincluded{$f}));

    my $children=($diby{$f} eq "")?0:1;
    my $count=1;

    if ($children)
    {
	my $t;
	foreach $t (sort split(/\s+/,$diby{$f}))
	{
	    # detect loops
	    if ($c!~/ $t /)
	    {
		$count+=countincluded($t, $l+1, $c." ".$t." ");
	    }
	}
    }
    $cincluded{$f}=$count;
    return $count;
}

sub bytarget
{
    my $x=($ctarget{$b}<=>$ctarget{$a});
    return $x?$x:($a<=>$b);
}

sub explaintarget
{
    my $f=shift @_;
    my $l=shift @_;
    my $c=shift @_;
    my $lh=sprintf("[%3d]",$l).("  " x $l);
    my $children=($di{$f} eq "")?0:1;
    if ($children)
    {
	printf "$lh $f which includes %d other files:\n",$ctarget{$f}-1;
	my $t;
	foreach $t (sort bytarget split(/\s+/,$di{$f}))
	{
	    # detect loops
	    if ($c=~/ $t /)
	    {
		print "$lh   $t (already included)\n";
	    }
	    else
	    {
		explaintarget($t, $l+1, $c." ".$t." ");
	    }
	}
    }
    else
    {
	print "$lh $f\n";
    }
}

sub byincluded
{
    my $x=($cincluded{$b}<=>$cincluded{$a});
    return $x?$x:($a<=>$b);
}

sub explainincluded
{
    my $f=shift @_;
    my $l=shift @_;
    my $c=shift @_;
    my $lh=sprintf("[%3d]",$l).("  " x $l);
    my $children=($diby{$f} eq "")?0:1;
    if ($children)
    {
	printf "$lh $f which is included by %d other files:\n",$cincluded{$f}-1;
	my $t;
	foreach $t (sort byincluded split(/\s+/,$diby{$f}))
	{
	    # detect loops
	    if ($c=~/ $t /)
	    {
		print "$lh   $t (already included)\n";
	    }
	    else
	    {
		explainincluded($t, $l+1, $c." ".$t." ");
	    }
	}
    }
    else
    {
	print "$lh $f\n";
    }
}

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
	    explaintarget($n,0) if ($explain);
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
	    explainincluded($n,0) if ($explain);
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

sub getfilename
{
    my $f = shift @_;
    return ($fnhash{$f}) if (defined($fnhash{$f}));

    my $ff = $f;
    $ff=~s/^lib\w+-//;
    $ff=~s/\.gch$//;
    $ff=~s/\.o$/.cpp/;
    my $ff2= $ff;
    $ff2=~s/\.cpp$/.c/;

    my $d;
    foreach $d (@dirs)
    {
	if (-e "$d/$ff")
	{
	    $fnhash{$f}="$d/$ff";
	    return $fnhash{$f};
	}
	if (-e "$d/$ff2")
	{
	    $fnhash{$f}="$d/$ff2";
	    return $fnhash{$f};
	}

    }
    print STDERR "Can't find file $f (looking for $ff)\n" if ($verbose);
    $fnhash{$f}="";
    return "";
}

sub processdirect
{
    my $f = shift @_;
    
    my $fn = getfilename($f);
    return if ($fn eq "");

    open (DEPFILE, $fn) || die ("Can't open deps file $fn - maybe you need to build first: $!");

    print STDERR "Processing $fn\n" if $verbose;
    
    my $firstinclude = 1;

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
	
        # Remove C++ comments
	$line=~s/\/\/.*$//;

	next unless $line=~ /^\s*#include\s+[\"\<](\w+\.h)[\"\>]/;

	my $d=basename($1);

	if ($firstinclude)
	{
	    $firstinclude = 0;
	    if ($warning && ($d ne "camtypes.h") && ($fn=~/.cpp$/) && ($fn!~/^wxXtra\//))
	    {
		print "$fn: Does not include camtypes.h first - has $d\n";
	    }
	}

	if (defined($includedependencies{$d}))
	{
	    print STDERR "  includes $d\n" if ($verbose>2);
	    $di{$f}.=" ".$d;
	}
    }
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
	    $depend{$dep}.="camtypes.h.gch " unless (($dep eq "camtypes.h.gch") || ($dep =~/\.h$/));

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

# Process files manually if necessary
if ($explain || $warning)
{
    my $f;
    my %seen=();
    my @allfiles=keys %targetdependencies;
    push @allfiles, keys %includedependencies;
    foreach $f (sort ( grep { ! $seen{$_} ++ } @allfiles ))
    {
	processdirect($f);
    }
    foreach $f (sort keys %di)
    {
	my $l=$di{$f};
	$l=~s/\s+/ /g;
	$l=~s/^\s+//g;
	$l=~s/\s+$//g;
	my @l=sort(split(/\s+/,$l));
	$di{$f}=join(" ",@l);
	my $i;
	foreach $i (@l)
	{
	    $diby{$i}.=" ".$f;
	}
    }
    foreach $f (sort keys %di)
    {
	print STDERR "Counting target $f\n" if ($verbose);
	counttarget($f);
    }

    print STDERR "Done counting targets\n" if ($verbose);

    foreach $f (sort keys %diby)
    {
	my $l=$diby{$f};
	$l=~s/\s+/ /g;
	$l=~s/^\s+//g;
	$l=~s/\s+$//g;
	my @l=sort(split(/\s+/,$l));
	$diby{$f}=join(" ",@l);
    }
    foreach $f (sort keys %diby)
    {
	print STDERR "Counting inclusion $f\n" if ($verbose);
	countincluded($f);
    }
    print STDERR "Done counting inclusions\n" if ($verbose);
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


