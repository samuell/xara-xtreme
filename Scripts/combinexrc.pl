#!/usr/bin/perl

# (c) 2005 Alex Bligh / Xara Group Ltd
#
# Combine one or more .xrc files
#
# Licensed under the GNU Public License - see LICENSE for
# more details

sub usage
{
    print STDERR <<EOF;

 combinexrc.pl - combine one or more .xrc files into a single .xrc file

 Usage:

 combinexrc.pl [options] [<filename> ...]

 "-" may be specified as the file to read stdin. Not specifying a file will
 result in stdin being read.

 The script does not glob filenames, but if you are calling from a normal shell
 you can use shell globs on the command line, e.g.:
   combinexrc.pl xrc/*.xrc > combined.xrc
 
 Output is written to standard output, unless -o is specified.

 The resources are simply concatenated, unless -s is specified, in which
 case each paramater is assumed to be a file containing a single stringtable,
 and a single stringtable is emitted consisting of all the entries in each
 sting table

 Options:
   -b / --bitmap <filename>        - filename to write for bitmaps that
                                     cannot be found in the same directory
                                     as the output file (if not specified,
                                     <bitmap> tags are not rewritten)
   -o <filename> / ---output <fn>  - write to the filename concerned
   -s / --stringtable              - concatenate string tables
   -t / --tab                      - do not write XML, but write a tab delimeted
                                     string table file
   --stdout                        - send to stdout, not outputfile (useful
                                     so you can still specify a path with -b).
   --verbose                       - be very loud about it
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

my $stdout = 0; 
my $help =0;
my $verbose = 0;
my $ilab = "";
my $inum = 0;
my $done = 0;
my $tab = 0;
my $outputfile="";
my $stringtable = 0;
my $bitmapname="";

my %tabtable;
my %tabtableplace;
my %tabtabledup;

GetOptions( "output|o=s" => \$outputfile,
	    "bitmap|b=s" => \$bitmapname,
	    "stdout!" => \$stdout,
	    "verbose|v+" => \$verbose,
	    "stringtable|s!" => \$stringtable,
	    "tab|t!" => \$tab,
	    "help!" => \$help ) || usage ("Bad option");

if ($help)
{
    usage();
}

# If no files specified, fake stdin as a file
push (@ARGV,"-") if ($#ARGV<0);

my $tostdout = 0;

my $ofn =">&1";

if (($outputfile eq "") || ($outputfile eq "-") || $stdout)
{
    $ofn=">&1";
    $tostdout = 1;
}
else
{
    $ofn = $outputfile;
}
    


open (OUTPUT, ">$ofn") || die ("Cannot open ouput file $ofn: $!");

# Write the header out.
if (!$tab)
{
    print OUTPUT "<?xml version=\"1.0\" ?>\n<resource>\n";
    if ($stringtable)
    {
	print OUTPUT "\t<object class=\"wxDialog\" name=\"STRINGTABLE\">\n";
	print OUTPUT "\t\t<title>String Table</title>\n";
	print OUTPUT "\t\t<object class=\"wxBoxSizer\">\n";
	print OUTPUT "\t\t\t<orient>wxVERTICAL</orient>\n";
    }
}

# Now iterate through each of the files specified on the command line

my $ifn;

foreach $ifn (@ARGV)
{
    if ($ifn eq "-")
    {
	$ifn="<&0";
    }

    # process the file
    process ($ifn, ($ifn eq "<&0")?"<STDIN>":$ifn);

}

if (!$tab)
{
    print OUTPUT "\t\t</object>\n\t</object>\n" if ($stringtable);
    print OUTPUT "</resource>\n";
}
else
{
    my $k;
    foreach $k (sort keys %tabtable)
    {
	printf OUTPUT "%s\t%s\n",$k,$tabtable{$k};
    }
}

close (OUTPUT);
    

exit (0);

sub important
{
    my $s=shift @_;
    return ($s=~/^-?$/)?0:1;
}

sub process
{
    my $in = shift @_;
    $ilab=shift @_;
    $inum=0;
    my %stringdefs;
    my $lastobjectname="";
    my @labelsent;

    print STDERR "$ilab: Starting processing\n" if ($verbose>1);

    open (INPUT, $in) || die ("Cannot open input file $in: $!");

    # bodge to remove duplicate checking on non-string table items, as
    # contols are allowed to share duplicate IDs between dialogs
    my $dupcheck=0;
    $dupcheck=1 if ($in=~/-strings.xrc$/);

    my $object=0;
    undef $labelsent[$object];

    while (<INPUT>)
    {
	$inum++;
	chomp;
	my $line = $_;
	# eliminate tags
	$line =~ s/<\?xml[^>]*>//g;
	$line =~ s/<(\/)?resource.*>//g;

	if ($stringtable)
	{
	    if ($line=~/<object/)
	    {
		# count the <object> references in the line (before the above mod)
		my $c;
		foreach $c (split(/s+/,$line))
		{
		    if ($c=~/<object/)
		    {
			# if this is the outermost object, delete the left most occurrence
			$line=~s/<object[^>]*>// if (!$tab && ($object<=1));
			$object++;
			undef $labelsent[$object];
			print STDERR "Found object level $object\n" if ($verbose>2);
		    }
		}
		if ($line=~/name\s*=\s*\"([^\"]+)\"/)
		{
		    my $string=$1;
		    $lastobjectname=$string;
		    print STDERR "Object level $object has name $string\n" if ($verbose>2);
		    $labelsent[$object]=$string;
		    if (defined($stringdefs{$string}) && $dupcheck)
		    {
			print STDERR "$ilab:$inum Warning $string multiply defined (local)\n";
			print STDERR $stringdefs{$string}." Previous definition here\n";
		    }
		    else
		    {
			$stringdefs{$string}="$ilab:$inum";
		    }
		}
		else
		{
		    print STDERR "Object has no name. Line: [$line]\n" if ($verbose>2);
		}
	    }

	    if ($tab && (defined($labelsent[$object])) &&
		($line =~ /<label>([^<]*)<\/label>/))
	    {
		my $label = $1;
		print STDERR "Found label for $object is $label\n" if ($verbose>2);
		tabstore($labelsent[$object], $label, $dupcheck);
		undef $labelsent[$object];
	    }

	
	    if ($line=~/<\/object>/)
	    {
		print STDERR "End object level $object\n" if ($verbose>2);
		# count the </object> references in the line (before the above mod)
		$lastobjectname="";
		my $c;
		foreach $c (split(/s+/,$line))
		{
		    if ($c =~ /<\/object>/)
		    {
			if ($tab && (defined($labelsent[$object])))
			{
			    tabstore($labelsent[$object], "-", $dupcheck);
			    undef $labelsent[$object];
			}

			$object-- ;
			
			# This line contained a </object> which reduced the count to zero
			# so delete it
			# BUG - we should delete the right-most not the left-most, but
			# as everyone seems to put these on a line of their own, we are OK.
			$line=~s/<\/object>// if (!$tab && ($object<=1));
		    }
		}
	    }

	    if ($object<=2)
	    {
		# delete title & position
		$line=~s/<title>.*<\/title>//;
		$line=~s/<pos>.*<\/pos>//;
		$line=~s/<size>.*<\/size>//;
		$line=~s/<(\w+)>.*<\/\1>//;
	    }
	}

	if (($bitmapname ne "") && ($line =~ /<bitmap>([^<]*)<\/bitmap>/))
	{
	    my $testb = $1;
	    my $basename = $outputfile;
	    # blank characters after the final slash
	    $basename =~ s/\/[^\/]+$/\//;
	    $testb = $basename.$testb if ($testb !~ /^\//);
	    if (! -e $testb)
	    {
		$line=~ s/<bitmap>([^<]*)<\/bitmap>/<bitmap>$bitmapname<\/bitmap>/;
		print STDERR "$ilab:$inum Warning: Bitmap $testb not found\n";
	    }
	}

	# Send out the line unless it is blank
	if (!$tab)
	{
	    print OUTPUT "$line\n" unless ($line =~ /^\s*$/);
	}
    }

    close (INPUT);

    return;
	
}

sub tabstore
{
    my $key = shift @_;
    my $label = shift @_;
    my $dupcheck = shift @_;

    if (!defined($tabtable{$key}))
    {
	$tabtable{$key}=$label;
	$tabtableplace{$key}="$ilab:$inum";
	$tabtabledup{$key}=$dupcheck;
    }
    else
    {
	# Only produce a warning if it's a duplicate with a label which is not empty or "-"
	if (1 || (important($label)) || important($tabtable{$key}))
	{
	    # And don't produce a warning unless one of them is in a string table file
	    if ($tabtabledup{$key} || $dupcheck)
			{
			    print STDERR "$ilab:$inum Warning $key multiply defined (global)\n";
			    print STDERR $tabtableplace{$key}." Previous definition here\n";
			}
	    $tabtabledup{$key}|=$dupcheck;
	}
    }
}
