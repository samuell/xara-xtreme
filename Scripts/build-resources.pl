#!/usr/bin/perl
# Scripts/build-resources.pl
# Build resources.cpp if necessary
# Call with a -f argument to force build of resources even if the system doesn't think it necessary

use strict;
use Fcntl ':mode';

sub usage
{
    print STDERR <<EOF;

 buildresources.pl - build camelot resources

 Usage:

 buildresources.pl [options]

 Options:
   -t TOPDIR           - Build source directory (for out of tree builds)
   -o OUTPUTDIR        - Build output directory (defaults to "wxOil")
   -f                  - Force rebuilding of resources and svnversion
   -x                  - setting of XARALANGUAGE
   -n / --version x.y  - set version to x.y ; omit to prevent svnversion
                         build
   -i | --international- build internationalisation resources ; omit to
                         prevent wxrc / xgettext etc usage
   -s                  - build svnversion
   --xgettext PATH     - pass alternate path to xgettext
   --wxrc PATH         - pass alternate path to wxrc
   --verbose           - be very loud about it
   --user              - setting of USERNAME
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

use strict;
use Getopt::Long;
use Digest::MD5;
use FileHandle;

my $context=new Digest::MD5;

my $help =0;
my $verbose = 0;
my $force=0;
my $topdir="";

my %tabtable;
my %tabtableplace;
my %tabtabledup;

my $xaralanguage="EN";
my $xgettext="xgettext";
my $wxrc="wxrc";
my $zip="zip";
my $outputdir="wxOil";
my $topdir=".";
my $version="";
my $user="unknown";
my $international=0;
my $svnversionenable=0;

GetOptions( "topdir|t=s" => \$topdir,
	    "ouputdir|o=s" => \$outputdir,
	    "force|f!" => \$force,
	    "international|i!" => \$international,
	    "svnversion|s!" => \$svnversionenable,
	    "verbose|v+" => \$verbose,
	    "version|n=s" => \$version,
	    "user|u=s" => \$user,
	    "xaralanguage|x=s" => \$xaralanguage,
	    "wxrc=s" => \$wxrc,
	    "xgettext|g=s" => \$xgettext,
	    "help!" => \$help ) || usage ("Bad option");

# workaround for Gentoo wxWidgets 2.6.3.2 missing wxrc
if ($wxrc eq "echo")
{
    print STRDERR "Warning: wxrc seems to be missing. Not building internationalized resources\n";
    $international=0;
}

usage() if ($help);

# check the output directory exists
mkdir ("$outputdir");
mkdir ("$outputdir/xrc");
mkdir ("$outputdir/xrc/$xaralanguage");

print STDERR "Testing for new resources and svn version\n";

# Resource system
# get the timestamp
my $omtime=0;
$omtime=(stat("$outputdir/resources.h"))[9]; # this may fail, in which case it looks like it was generated at the epoch
$omtime+=0;

my $potime=0;
if ($international)
{
    # Use the xaralx.po time if it's earlier, to force a rebuild (common case - it's missing)
    $potime=(stat("$outputdir/xrc/xaralx.po"))[9]; # this may fail, in which case it looks like it was generated at the epoch
    $potime+=0;
    $omtime=$potime if ($potime<$omtime);
}

# And the same thing for svnversion - we look at the cached file states. When we rebuild the cache we check
# to see if this would result in a different svnversion. The .h file might not change as not every cache file change
# results in a new svnversion.
my $svtime=0;
$svtime=(stat("$outputdir/svnversion.cache"))[9]; # this may fail, in which case it looks like it was generated at the epoch
$svtime+=0;

opendir(DIR, "$topdir/wxOil/xrc") || die "Can't open $topdir/wxOil/xrc: $!";
my @resfiles=sort grep { /^[^\.].*\.(png|ico|cur|bmp|res|xar)$/ } readdir(DIR);
closedir(DIR);

opendir(DIR, "$topdir/wxOil/xrc/$xaralanguage") || die "Can't open $topdir/wxOil/xrc/$xaralanguage: $!";
my @xrcfiles=sort grep { /^[^\.].*\.xrc$/ } readdir(DIR);
closedir(DIR);

my $newer=0;
my $svnewer=0;
my $ponewer=0;

my $f;
foreach $f (@xrcfiles)
{
    $f = "$topdir/wxOil/xrc/$xaralanguage/".$f;
    if ((stat($f))[9] > $omtime)
    {
	$newer=1;
	$svnewer=1;
    }
    if ((stat($f))[9] > $svtime)
    {
	$svnewer=1;
    }
    if ((stat($f))[9] > $potime)
    {
	$ponewer=1;
    }
}

foreach $f (@resfiles)
{
    $f = "$topdir/wxOil/xrc/".$f;
    if ((stat($f))[9] > $omtime)
    {
	$newer=1;
	$svnewer=1;
    }
    if ((stat($f))[9] > $svtime)
    {
	$svnewer=1;
    }
}

if ((stat("$topdir/wxOil/xrc/$xaralanguage"))[9] > $omtime)
{
    $svnewer=1;
    $newer=1;
}
if ((stat("$topdir/wxOil/xrc/$xaralanguage"))[9] > $svtime)
{
    $svnewer=1;
}
if ((stat("$topdir/wxOil/xrc"))[9] > $omtime)
{
    $svnewer=1;
    $newer=1;
}
if ((stat("$topdir/wxOil/xrc"))[9] > $svtime)
{
    $svnewer=1;
}

# Firstly, let's find the svn version if one was specified (else we leave it)
if ($version ne "")
{
    if ($svnversionenable)
    {
	# First determine whether we need to do a rebuild of svnversion at all
	# We look to see if any of the files passed on the command line are
	# newer than svnversion.h
	
	if (open(VCACHE,"$outputdir/svnversion.cache"))
	{
	    while(<VCACHE>)
	    {
		next if /^\?/; # miss stuff not under version control
		# Note svn status prepends $topdir unless it's "." in which case it is not needed
		$f=substr($_, 40, -1);
		my @s;
		@s=stat($f);
		my $m;
		$m=$s[9]+0;
		next if (S_ISDIR($s[2])); # ignore directories
		print STDERR "Checking '$f' - mtime $m\n" if ($verbose >1);
		if (($m == 0) || ($m > $svtime)) # catch case where file disappears
		{
		    $svnewer=1;
		    print STDERR "$f is NEWER, $m > $svtime \n" if ($verbose);
		}
	    }
	    close (VCACHE);
	}
	else
	{
	    # No cache file
	    $svnewer = 1;
	}
    }
    else
    {
	# svnversion not enabled - we still have to generate something
	$svnewer = 1;
    }
	
	
    my $writeversion;
    my @svnversion;
    $writeversion=0;

    if ($svnewer || $force)
    {
	my $svnv;
	my $bdate;
	if ($svnversionenable)
	{
	    print STDERR "Rebuilding svn version\n";
	    
	    # Generate the cache
	    system ("svn status -v $topdir > $outputdir/svnversion.cache");
	    
	    $svnv=`svnversion $topdir`;
	    $bdate= `date +"%d-%b-%y %H:%M"`;
	    chomp($bdate);
	    chomp($svnv);
	    print STDERR "svnversion gives $svnv\n";
	}
	else
	{
	    # erase the cache so if versioning is turned back on, it recreates it
	    unlink("$outputdir/svnversion.cache");
	   
	    $svnv="0000";
	    $bdate="00-00-00 00:00";
	}

	my $camversionmajor;
	my $camversionminor;
	($camversionmajor, $camversionminor)=split('.',$version);
	push @svnversion, 'const TCHAR g_pszSvnVersion[] = wxT("'.$svnv.'");';
	push @svnversion, 'const TCHAR g_pszAppVersion[] = wxT("'.$version.'");';
	push @svnversion, "#define CAMELOT_VERSION_MAJOR $camversionmajor";
	push @svnversion, "#define CAMELOT_VERSION_MINOR $camversionminor";
	push @svnversion, "#define CAMELOT_VERSION $version";
	push @svnversion, "#define CAMELOT_VERSION_STRING wxT(\"$version ($user)\")";
	push @svnversion, "#define CAMELOT_BUILD_DATE _T(\"$bdate\")";
	
	# in case it doesn't open
	$writeversion=1;
	
	my $i=0;
	if (open(VERSION,"$outputdir/svnversion.h"))
	{
	    $writeversion=$force;
	    while(<VERSION>)
	    {
		chomp;
		my $l;
		$l=$svnversion[$i++];
		# ignore differences in date, or every make would produce a new link!
		if (!((/CAMELOT_BUILD_DATE/) && ($l=~/CAMELOT_BUILD_DATE/)) && ($l ne $_))
		{
		    $writeversion=1;
		    last;
		}
	    }
	}
    }

    # OK it's changed
    if ($writeversion)
    {
	print STDERR "Writing svnversion.h\n";
	open (VERSION, ">$outputdir/svnversion.h") || die "Can't write svnversion.h: $!";
	foreach my $i (@svnversion)
	{
	    print VERSION "$i\n";
	}
	close(VERSION);
    }
}

# If there are no newer files, and force isn't set, exit without even doing the checksum
if (!$newer && !$force)
{
    print STDERR "Nothing new\n";
    ok();
    exit (0);
}

my @dialogfiles = sort grep { $_ !~ /-strings\.xrc$/ } @xrcfiles;
my @stringfiles = sort grep { $_ =~ /-strings\.xrc$/ } @xrcfiles;

my @all=@resfiles;
push @all,@xrcfiles;
my $i;
foreach $i (@all)
{
    print STDERR "Checksumming $i\n" if ($verbose>1);
    my $fh=new FileHandle "<$i";
    if (!defined($fh))
    {
	die "Can't open $i for checksumming: $!";
    }
    $context->add($i); # add the name of the file too
    $context->addfile($fh);
}

my $checksum=$context->hexdigest();

print STDERR "New checksum = $checksum\n" if ($verbose);

my $buildresources=$force;

if (!$buildresources)
{
    if (open(CHECK,"$outputdir/xrc/xrc.check"))
    {
	$buildresources=1;
	while(<CHECK>)
	{
	    chomp;
	    if ($_ eq $checksum)
	    {
		$buildresources=0;
		last;
	    }
	}
    }
    else
    {
	$buildresources=1;
    }
}

my $ret;

if ($buildresources)
{
    print STDERR "Rebuilding resources - new checksum $checksum\n";
    unlink ("$outputdir/xrc/$xaralanguage/xrc.check");
    unlink ("$outputdir/xrc/xrc.check");

    $ret=system("$topdir/Scripts/combinexrc.pl","-b","missing.png","-o","$outputdir/xrc/dialogs.xrc",@dialogfiles);
    die "Failed to combine dialogs ($ret): $!" if ($ret);
    $ret=system("$topdir/Scripts/combinexrc.pl","-b","missing.png","-t","-s","-o","$outputdir/xrc/strings.lst",@xrcfiles);
    die "Failed to combine strings ($ret): $!" if ($ret);
}

if ($international && ($buildresources || $ponewer))
{
    my @strings;
    open(STRINGS,"$outputdir/xrc/strings.lst") || die "Could not open strings.lst: $!";
    while (<STRINGS>)
    {
	chomp;
	s/^\S+\t//;
	# escape slashes
	s/\\/\\\\/g;
	# escape quotes
	s/\"/\\\"/g;
	my $s;
	$s="_(\"$_\");";
	push @strings, $s;
	print STDERR "String: $s\n" if ($verbose>2);
    }
    close(STRINGS);
    
    my $wxrccommand;
    $wxrccommand=$wxrc." -g $outputdir/xrc/dialogs.xrc";
    if ($wxrc=~/ /)
    {
	# Some installations have an "eval" type command to set LD_PATH
	$wxrccommand="/bin/sh -c '".$wxrccommand."'";
    }

    my $dlines=0;
    open(DIALOGS,"$wxrccommand|") || die "Could not read dialogs for translation [$wxrccommand]: $!";
    while (<DIALOGS>)
    {
	# Note wxrc removes XML escaping
	chomp;
	print STDERR "Dialog: $_\n" if ($verbose>2);
	push @strings,$_;
	$dlines++;
    }
    close(DIALOGS);
    
    die "Could not read dialogs for translation (empty or bad wxrc) [$wxrccommand]" if ($dlines<2);

    my @uniqstrings;
    my $last="";
    foreach $i (sort @strings)
    {
	next if ($i eq $last);
	$last = $i;
	my $j;
	$j=$i;
	next if ($j =~ /^_\(\"\"\)\;\s+$/);
	$j=~s/\\r\\n/\\n/g;
	# strings.lst is still XML escaped. We want to remove the XML escaping here - we add C escaping to BOTH
	# later
	# We should use proper XML unquoting here - AMB to fix - could use HTML::Entities if it's around
	# Handle quoted numbers
	my $c;
	$j=~s/(&\#(\d+);?)/$2 < 256 ? chr($2) : $1/eg;
	$j=~s/(&\#[xX]([0-9a-fA-F]+);?)/$c = hex($2); $c < 256 ? chr($c) : $1/eg;
	$j=~s/&lt;/\</g;
	$j=~s/&gt;/\>/g;
	$j=~s/&quot;/\"/g;
	$j=~s/&amp;/\&/g;
	push @uniqstrings, $j;
    }
    
    my $n=1;
    open (XGETTEXT, "|".$xgettext.' --from-code ISO-8859-1 --force-po -k_ -C -i - --no-location --copyright-holder "Xara Group Ltd" --msgid-bugs-address=bugs@xara.com -d xaralx -o '.$outputdir."/xrc/xaralx.po") || die "Can't run $xgettext: $!";
    foreach $i (@uniqstrings)
    {
	print STDERR "Line $n, translate: $i\n" if ($verbose>2);
	$n++;
	print XGETTEXT "$i\n";
    }
}

# Write the file to the wrong directory (deliberate)
open(CHECK,">$outputdir/xrc/$xaralanguage/xrc.check") || die "Could not write xrc.check: $!";
print CHECK "$checksum\n";
close CHECK;

$ret = system("$zip","-9","-j","-q","$outputdir/xrc/resources.xrs","$outputdir/xrc/$xaralanguage/xrc.check","$outputdir/xrc/dialogs.xrc","$outputdir/xrc/strings.xrc","$outputdir/xrc/strings.lst",@resfiles);
die "Could not make resources.xrs: $!" if ($ret);
$ret = system("$topdir/Scripts/bin2cpp.pl -f CamResource::GetBinaryFileInfo $outputdir/xrc/resources.xrs $outputdir/resources.h");
die "Could not make resources.cpp: $!" if ($ret);

# OK we've finished so move the checksum into the right place
rename ("$outputdir/xrc/$xaralanguage/xrc.check", "$outputdir/xrc/xrc.check");

ok();
exit(0);

# Print an OK to STDOUT
sub ok
{
    print "OK\n";
}
