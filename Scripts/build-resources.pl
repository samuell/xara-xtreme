#!/usr/bin/perl
# Scripts/build-resources.pl
# Build resources.cpp if necessary
# Call with a -f argument to force build of resources even if the system doesn't think it necessary

use strict;

sub usage
{
    print STDERR <<EOF;

 buildresources.pl - build camelot resources

 Usage:

 buildresources.pl [options]

 Options:
   -t TOPDIR                       - Build source directory (for out of tree builds)
   -o OUTPUTDIR                    - Build output directory (defaults to "wxOil")
   -f                              - Force rebuilding of resources
   -x                              - setting of XARALANGUAGE
   -n / --version x.y              - set version to x.y
   --verbose                       - be very loud about it
   --user                          - setting of USERNAME
   --help                          - display this message

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

my $checksum="md5sum";
my $xaralanguage="EN";
my $xgettext="xgettext";
my $wxrc="wxrc";
my $zip="zip";
my $outputdir="wxOil";
my $topdir=".";
my $version="";
my $user="unknown";

GetOptions( "topdir|t=s" => \$topdir,
	    "ouputdir|o=s" => \$outputdir,
	    "force!" => \$force,
	    "verbose|v+" => \$verbose,
	    "version|n=s" => \$version,
	    "user|u=s" => \$user,
	    "xaralanguage|x=s" => \$xaralanguage,
	    "help!" => \$help ) || usage ("Bad option");

# check the output directory exists
mkdir ("$outputdir");
mkdir ("$outputdir/xrc");
mkdir ("$outputdir/xrc/$xaralanguage");

print STDERR "Testing for new resources and svn version\n";

# Firstly, let's find the svn version if one was specified (else we leave it)
if ($version ne "")
{
    my $svnv;
    $svnv=`svnversion $topdir`;
    my $bdate;
    $bdate= `date +"%d-%b-%y %H:%M"`;
    chomp($bdate);
    chomp($svnv);
    my @svnversion;
    my $camversionmajor;
    my $camversionminor;
    ($camversionmajor, $camversionminor)=split('.',$version);
    push @svnversion, 'const TCHAR g_pszSvnVersion[] = wxT("'.$svnv.'");';
    push @svnversion, 'const TCHAR g_pszAppVersion[] = wxT("'.$version.'");';
    push @svnversion, "#define CAMELOT_VERSION_MAJOR $camversionmajor";
    push @svnversion, "#define CAMELOT_VERSION_MINOR $camversionminor";
    push @svnversion, "#define CAMELOT_VERSION $version";
    push @svnversion, "#define CAMELOT_VERSION_STRING wxT(\"$version ($user)\")";
    push @svnversion, "#define CAMELOT_BUILD_DATE _T(\"$bdate\")".

    my $writeversion;
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

    # OK it's changed
    if ($writeversion)
    {
	print STDERR "Writing svnversion.h\n";
	open (VERSION, ">$outputdir/svnversion.h") || die "Can't write svnversion.h: $!";
	foreach $i (@svnversion)
	{
	    print VERSION "$i\n";
	}
	close(VERSION);
    }
}


# Resource system
# We don't bother with timestamps. Just as quick to mdsum these things like subversion does

opendir(DIR, "$topdir/wxOil/xrc") || die "Can't open $topdir/wxOil/xrc: $!";
my @resfiles=sort grep { /^[^\#].*\.(png|ico|cur|bmp|res|xar)$/ } readdir(DIR);
closedir(DIR);

opendir(DIR, "$topdir/wxOil/xrc/$xaralanguage") || die "Can't open $topdir/wxOil/xrc/$xaralanguage: $!";
my @xrcfiles=sort grep { /^[^\#].*\.xrc$/ } readdir(DIR);
closedir(DIR);

my $f;
foreach $f (@xrcfiles)
{
    $f = "$topdir/wxOil/xrc/$xaralanguage/".$f;
}
foreach $f (@resfiles)
{
    $f = "$topdir/wxOil/xrc//".$f;
}

my @dialogfiles = sort grep { $_ !~ /-strings\.xrc$/ } @xrcfiles;
my @stringfiles = sort grep { $_ =~ /-strings\.xrc$/ } @xrcfiles;

my @all=@resfiles;
push @all,@xrcfiles;
my $i;
foreach $i (@all)
{
    print STDERR "Checksumming $i\n" if ($verbose);
    my $fh=new FileHandle "<$i";
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

exit(0) unless $buildresources;

print STDERR "Rebuilding resources - new checksum $checksum\n";
unlink ("$outputdir/xrc/$xaralanguage/xrc.check");
unlink ("$outputdir/xrc/xrc.check");

my $ret;
$ret=system("$topdir/Scripts/combinexrc.pl","-b","missing.png","-o","$outputdir/xrc/dialogs.xrc",@dialogfiles);
die "Failed to combine dialogs ($ret): $!" if ($ret);
$ret=system("$topdir/Scripts/combinexrc.pl","-b","missing.png","-t","-s","-o","$outputdir/xrc/strings.lst",@xrcfiles);
die "Failed to combine strings ($ret): $!" if ($ret);

my @strings;
open(STRINGS,"$outputdir/xrc/strings.lst") || die "Could not open strings.lst: $!";
while (<STRINGS>)
{
    chomp;
    s/^\S+\t//;
    my $s;
    $s="_(\"$_\");";
    push @strings, $s;
    print STDERR "String: $s\n" if ($verbose>2);
}
close(STRINGS);

open(DIALOGS,"$wxrc -g $outputdir/xrc/dialogs.xrc|") || die "Could not read dialogs for translation: $!";
while (<DIALOGS>)
{
    chomp;
    print STDERR "Dialog: $_\n" if ($verbose>2);
    push @strings,$_;
}

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
#    $j=~s/&amp;/\&/g;
    push @uniqstrings, $j;
}

open (XGETTEXT, "|".$xgettext.' --force-po -k_ -C -i - --no-location --copyright-holder "Xara Group Ltd" --msgid-bugs-address=bugs@xara.com -d xaralx -o '.$outputdir."/xrc/xaralx.po") || die "Can't run $xgettext: $!";
foreach $i (@uniqstrings)
{
    print STDERR "Translate: $i\n" if ($verbose>2);
    print XGETTEXT "$i\n";
}
close (XGETTEXT);

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

exit(0);
