#!/usr/bin/perl

sub usage
{
    print STDERR <<EOF;

 normalise.pl - a script to convert historic Camelot files into a format suitable
                for open-source release

 Usage:

 normalise.pl [options] [<filename> ...]

 "-" may be specified as the file to read stdin. Not specifying a file will
 result in stdin being read.

 The script does not glob filenames, but if you are calling from a normal shell
 you can use shell globs on the command line, e.g.:
   normalise.pl Kernel/*.{cpp,h} wxOil/*.{cpp,h}

 Files are not touched unless there is a change made, in which case a new file
 is moved over the old file. This means the you can safely do (for instance)
   cd ..
   cp -al MyTree TestTree     # <---- makes links
   cd TestTree
   find . -name '*.cpp' -or -name '*.h' | xargs Scripts/normalise.pl

 The following normalisations are currenly supported
 * Fix Newlines - remove carriage returns and ensure a final newline exists
 * Fix Authors  - change pre-release camelot author identification into something
                  meaningful to the rest of the world
 * Fix Types    - change archicture dependent deprecated types to new format
                  (note this does not fix ALL compiler native types, but these
                  weren't in the old source anyway)
 * Fix Notice   - insert a standard copyright message in the file, or if
                  one is already present, ensure it is up to date.

 The "authors" file is a comma delimited file of the following format:
   <NameToReplace>,<FullName>,<Organisation>,<EmailAddress>

 You can get a list of unknown authors by:
   Scripts/normalise.pl -v --dry-run 2>&1 | fgrep "Unknown author" [files...]

 The copyright notice file should NOT contain the leading "/*" or the trailing
 "*/". The script inserts these itself. Please do not play with this bit of the
 script.

 The script will happily support you changing the copyright notice, and
 removing the old one (if it is wrong) and replacing it - it sees this just
 as if someone had altered the copyright notice (which we do not want them to
 do).

 Options:
   --fixnl    / --no-fixnl         - strip out carriage returns, make
                                     sure file ends in a newline (default: on)
   --fixauthors / --no-fixauthors  - substitute deprecated author codes
                                     with email addresses etc. (default: on)
   --fixtypes / --no-fixtypes      - change references to deprecated types
                                     to new types (default: on)
   --fixnotice / --no-fixnotice    - put in a copyright notice at the head of
                                     the file
                                     (default: on)
   --fixtrace / --no-fixtrace      - fix trace statements (default: on)
   --fixoptoken / --no-fixoptoken  - fix optoken handling (default: on)
   --fixresource / --no-fixresource- fix resource identifiers (default: on)
   --fixralph / --no-fixralph      - fix RALPH exclusions to also exclude
                                     XaraLX (default: off). DO NOT USE
                                     UNLESS YOU KNOW WHAT YOU ARE DOING
   --fixsvn / --no-fixsvn          - Remove RCS headers and insert a subversion header
                                     (default: on).
   --fixkeys / --no-fixkeys        - Turn VK_FOO into CAMKEY(FOO)
                                     (default: on).
   --backup / --no-backup          - create backup files (default: on)
   --authorfile=<FILE>             - use author translation table file
                                     specified
                                     (defaults to Scripts/NameTable.txt)
   --noticefile=<FILE>             - file containing the copyright notice
                                     (defaults to LICENSE)
				     This will be enclosed AUTOMATICALLY by
                                     this program within /* ... */, and the
                                     identifying lines saying "Do not remove
                                     this notice" (which are used to detect
                                     it) are hardcoded in here.
   --tempext=<extension>           - use specified file extension for
                                     temporary files
   --backupext=<extension>         - use specified file extension for backup
                                     files
   --dry-run                       - create temp file, then delete it,
                                     do not apply changes
   --stdout                        - write the translated file to
                                     standard output (not the temp file)
   --verbose                       - be very loud about it
   --help                          - display this message
   --removeinclude                 - remove the specified include

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

my $fixnl = 1;
my $fixauthors = 1;
my $fixtypes = 1;
my $fixnotice = 1;
my $fixtrace = 1;
my $fixresource = 1;
my $fixoptoken = 1;
my $fixralph = 0;
my $fixsvn = 1;
my $fixkeys = 1;
my $authorfile = 'Scripts/NameTable.txt';
my $noticefile = 'LICENSE';
my $dryrun = 0;
my $stdout = 0; 
my $help =0;
my $verbose = 0;
my $tempext = ".$$.tmp";
my $backupext = '.bak';
my $backup = 1;
my @removeinclude;

my $noticestart = '/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE';
my $noticeend   = ' */';
my $noticeskip  = '/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */';

my %authors;
my %types;
my $newnotice;
my @kauthors;
my @ktypes;
my $removeincludepat;

GetOptions( "fixnl!" => \$fixnl,
	    "fixauthors!" => \$fixauthors,
	    "fixtypes!" => \$fixtypes,
	    "fixnotice!" => \$fixnotice,
	    "fixtrace!" => \$fixtrace,
	    "fixresource!" => \$fixresource,
	    "fixoptoken!" => \$fixoptoken,
	    "fixralph!" => \$fixralph,
	    "fixsvn!" => \$fixsvn,
	    "fixkeys!" => \$fixkeys,
	    "authorfile=s" => \$authorfile,
	    "noticefile=s" => \$noticefile,
	    "dry-run!" => \$dryrun,
	    "stdout!" => \$stdout,
	    "verbose|v+" => \$verbose,
	    "tempext=s" => \$tempext,
	    "backupext=s" => \$backupext,
	    "backup!" => \$backup,
	    "removeinclude=s" => \@removeinclude,
	    "help!" => \$help ) || usage ("Bad option");

if ($help)
{
    usage();
}

# If no files specified, fake stdin as a file
push (@ARGV,"-") if ($#ARGV<0);

readstrings();

# Now iterate through each of the files specified on the command line

my $ifn;


foreach $ifn (@ARGV)
{
    my $ofn = $ifn.$tempext;

    my $output = do { local *INPUT };
    my $input =  do { local *OUTPUT };
    my $tostdout = 0;

    if ($ifn eq "-")
    {
	$ifn="<&0";
	$ofn=">&1";
	$tostdout = 1;
    }

    if ($stdout)
    {
	$ofn=">&1";
	$tostdout = 1;
    }

    # process the file
    my $altered = process ($ifn, $ofn, ($ifn eq "<&0")?"<STDIN>":$ifn);

    # Now, if we weren't writing to stdout, clear up the files
    if (!$tostdout)
    {
	if ($altered && !$dryrun)
	{
	    if ($backup)
	    {
		rename ($ifn, $ifn.$backupext) || die ("Cannot make backup file: $!");
	    }
	    rename ($ofn, $ifn) || die ("Cannot overwrite original file: $!");
	}
	else
	{
	    unlink ($ofn) || die ("Can't unlink temp file: $!");
	}
    }
}

exit (0);

# make a pattern that matches a notice start or end
sub makenpat
{
    my $t=shift @_;
    # put a backslash in front of various things
    $t=~s/([[:punct:]])/\\\1/g;
    return( qr/^$t/ );
}

sub process
{
    my $in = shift @_;
    my $out = shift @_;
    my $ilab = shift @_;
    my $line;
    my $buff;
    my $changed=0;

    my $preamble = 1;
    my $buffering=$fixnotice?1:0;
    my $sbuff="";

    my $noticespat;
    my $noticeepat;
    my $noticeskippat;
    my $skipfixtypes=0;

    my $inum=0;

    print STDERR "$ilab: Starting processing\n" if ($verbose>0);

    print STDERR "$ilab: Buffering is $buffering\n" if ($verbose>2);

    # Preload the buffer with what we want to print
    if ($fixnotice)
    {
		$noticespat = makenpat($noticestart);
		$noticeepat = qr/\*\//; # allow any line with a close of comment
		$noticeskippat = makenpat($noticeskip);
		$sbuff = $noticestart."\n".$newnotice.$noticeend."\n";
    }

    $buff=$sbuff;

    open (INPUT, $in) || die ("Cannot open input file: $!");
    open (OUTPUT, ">$out") || die ("Cannot open ouput file: $!");

    while ($line=<INPUT>)
    {
	$inum++;
	my $origline = $line;

	if ($fixnl)
	{
	    chomp ($line);
	    $line=~s/\r//g;
	    $line=$line."\n";
	}

	# Fix subversion headers
	if ($fixsvn)
	{
	    if ($line=~ /^\/\/\s*\$Header:\s*.*\$\s*$/)
	    {
		$line=""; # Not even a \n - we want to delete it
		$changed=1;
	    }
	    if (($inum == 1) && ($line !~ /^\/\/\s*\$Id:\s*.*\$\s*$/))
	    {
		my $iline="// \$Id: $ilab, 1, 01-Jan-2006, Anonymous \$\n";
		$changed=1;

		# Squeeze copy of the header in right at the top
		if ($buffering && !$preamble)
		{
		    $buff=$iline.$buff;
		}
		else
		{
		    print OUTPUT $iline;
		}
	    }
	}

	# Remove unwanted includes
	if (scalar(@removeinclude))
	{
	    if ($line =~ /$removeincludepat/)
	    {
		chomp($line);
		$line="//$line - in camtypes.h [AUTOMATICALLY REMOVED]\n";
	    }
	}

	# Fix trace statements to quote the string in _T if it's not
	# there already
	if (($fixtrace) && ($line !~ /\b_T\(/))
	{
	    $line=~ s,\bTRACE\s*\(\s*(".*"),TRACE\( _T(\1),g;
	    $line=~ s,\bTRACE0\s*\(\s*(".*"),TRACE0\( _T(\1),g;
	    $line=~ s,\bTRACEALL\s*\(\s*(".*"),TRACEALL\( _T(\1),g;
	    $line=~ s-\bTRACEUSER\s*\(\s*(.*)\s*,\s*(".*")-TRACEUSER\( \1, _T\(\2\)-g;
	}

	if ($fixkeys && !$skipfixtypes) # hang on this to avoid break wxXtra
	{
	    $line=~ s/\bVK_(\w+)\b/CAMKEY\(\1\)/;
	}

	# Replace #define of OPTOKEN stuff to include _T("")
	# eliminate broken usage of wxT() which is incompatible with kernel code
	if (($fixoptoken) && ($line !~ /\b_T\(/))
	{
	    if ( $line =~ /^\s*\#define\s+OPTOKEN_/ )
	    {
		$line=~ s,^(\s*\#define\s+OPTOKEN_\w+\s+)(\".*\")(.*)$,\1_T\(\2\)\3,g;
	    }
	    else
	    {
		$line=~ s/wxT\s*\(\s*(OPTOKEN_\w+)\s*\)/\1/g;
	    }
	}

	if (($fixralph) && ($line =~ /\bEXCLUDE_FROM_RALPH\b/) && ($line !~ /\bEXCLUDE_FROM_XARALX\b/) && ($line !~ /^\s*\#define/))
	{
	    $line =~ s/^(\s*)\#ifndef\s+EXCLUDE_FROM_RALPH/\1\#if !defined(EXCLUDE_FROM_RALPH) && !defined(EXCLUDE_FROM_XARALX)/;
	    $line =~ s/^(\s*)\#ifdef\s+EXCLUDE_FROM_RALPH/\1\#if defined(EXCLUDE_FROM_RALPH) || defined(EXCLUDE_FROM_XARALX)/;
	    $line =~ s/^(\s*)\#endif.*EXCLUDE_FROM_RALPH\s*\n$/\1\#endif \/\/ EXCLUDE_FROM_RALPH, EXCLUDE_FROM_XARALX\n/;
	}

	if (($fixralph) && ($line !~ /PORTNOTE/))
	{
	    $line =~ s/^(\s*)\#pragma\s+message\s*\(\s*__LOCMSG__\s+((_T\(\s*)?)(\"[^\"]+\")(\s*\))*\s*$/\1PORTNOTE("other",\4)\n/;
	}

	# Fix resource identifiers by quoting with the _R macro
	if (($fixresource) && 
	    ( ($line =~ /\bID([A-Z]*)_\w+\b/) || ($line =~ /\b[A-Z]+TOOLDLG\b/) || ($line =~ /\bAFX_ID[A-Z]_\w+\b/))
	    && ($line !~ /^\s*\#define/) && ($line !~ /\bNORESOURCEFIX\b/))
	{
	    # OK, we've got a likely line. But we can't just quote IDD_FOOBAR etc.
	    # because we'll end up double quoting them to infinity. So we use a sneaky
	    # trick. Quote them, then remove doubles, as these are always invalid
	    $line =~ s/\b(ID[A-Z]*_\w+)\b/_R(\1)/g;
	    $line =~ s/\b(AFX_ID[A-Z]_\w+)\b/_R(\1)/g;
	    $line =~ s/\b([A-Z]+TOOLDLG)\b/_R(\1)/g;
	    $line =~ s/_R\((_R\(\s*\w+\s*\))(\s*)\)/\1\2/g;
	    # and unquote things with more than 4 letters
	    $line =~ s/_R\((ID[A-Z][A-Z][A-Z][A-Z][A-Z]+_\w+)\)/\1/g;
	}
	
	if ($fixresource && ($line=~/^s*\#include/) && ($line !~ /\bNORESOURCEFIX\b/))
	{
	    if ($line=~ /\b(aboutres\.h|accures\.h|alex\.h|andydlg\.h|andy\.h|andyh\.h|assoc\.h|barsdlgs\.h|ben\.h|bevres\.h|bezbar\.h|bfxrc\.h|biasres\.h|blendbar\.h|blndres\.h|blnkbar\.h|blobtab\.h|bmpdlg\.h|bmpdlgr\.h|bmpreres\.h|bmpres\.h|bmpsdlgr\.h|brdlgres\.h|brushres\.h|ccprpres\.h|chris\.h|clikdrag\.h|clipdlg\.h|clipres\.h|cmxres\.h|cntres\.h|cntrres\.h|colordlg\.h|combbuts\.h|customcontrols\.h|dbgdlg\.h|eddlg\.h|ed\.h|errordlg\.h|exprwres\.h|extendres\.h|feather\.h|filtrres\.h|freehres\.h|galres\.h|galstr\.h|gerry\.h|gifdlg\.h|helppath\.h|hotdlg\.h|hotlink\.h|imagdlg\.h|isetres\.h|jason\.h|jim\.h|justin2\.h|justin3\.h|justin\.h|linegal\.h|liveeffectres\.h|lyrprop\.h|mario\.h|markg\.h|markn\.h|mike\.h|mouldbar\.h|nev\.h|nwcolres\.h|ollie\.h|oplddir\.h|optsres\.h|pagesres\.h|penbar\.h|peterdlg\.h|peter\.h|phil\.h|plugres\.h|prefsdlg\.h|prevwres\.h|printdlg\.h|printstr\.h|register\.h|resimmap\.h|resource\.h|richard2\.h|richard3\.h|richard\.h|rikdlg\.h|rik\.h|rotatini\.h|rshapbar\.h|selstr\.h|shadbar\.h|shadres\.h|simon\.h|sliceres\.h|tbbitmap\.h|textres\.h|timdlg\.h|tim\.h|tips\.h|tipsres\.h|tlockres\.h|tmpltres\.h|tracerc\.h|urlres\.h|urls\.h|viewrc\.h|webdialog\.h|webdlg\.h|webprvw\.h|webster\.h|will2\.h|will3\.h|will\.h|xardlg\.h|xpfres\.h|zoomres\.h)\b/ )
	    {
		$line="//".$line;
	    }
	}

	if ($fixauthors && ($line =~ /^[ \t]*Author:[ \t]*\w+/))
	{
	    # First, we must avoid fixing something which is already fixed.
	    # We use the heuristic that if it has a valid looking email address
	    # in the line, we probably already fixed it.

	    if ($line !~ /\S+@\S+\.\S+/ )
	    {
		my $found=0;
		my $pat;
		for $pat (@kauthors)
		{
		    my $rep=$authors{$pat};
		    print STDERR "$ilab:$inum: Trying $pat \n" if ($verbose>3);
		    if ($line =~ s/$pat/\1$rep/g)
		    {
			if ($verbose>2)
			{
			    print STDERR "$ilab:$inum: Replacing $pat with $rep\n";
			}
			$found = 1;
			last; # Only replace one
		    }
		}
		if (!$found && $verbose>0)
		{
		    my $uauthor = $line;
		    if ($uauthor =~ s/^[ \t]*Author:[ \t]*\b(.*)\b\s*$/\1/ )
		    {
			printf STDERR "$ilab:$inum: Unknown author: %s\n",$uauthor;
		    }
		}
	    }
	}

	$skipfixtypes = 1 if ($line =~ /SKIPFIXTYPES: START/);
	$skipfixtypes = 0 if ($line =~ /SKIPFIXTYPES: END/);

	if (($fixtypes) && (!$skipfixtypes) && ($line !~ /TYPENOTE:/))
	{
	    my $pat;
	    for $pat (@ktypes)
	    {
		my $rep=$types{$pat};
		print STDERR "$ilab:$inum: Trying $pat \n" if ($verbose>3);
		if ($line =~ s/$pat/$rep/g)
		{
		    if ($verbose>2)
		    {
			print STDERR "$ilab:$inum: Replacing $pat with $rep\n";
		    }
		}
	    }

	    # We convert the pattern 'long' manually - this is because it is a heuristic conversion
	    # we don't want to convert 'see if the string is too long' etc.
	    if ($line =~ /\blong\b/)
	    {
		# lines beginning with just the word "long" convert
		# $line =~ s/(^\s+)long\b/\1INT32\g;
		
		# now go through each occurrence of the word 'long' on the line
		my @bits;
		my $new;
		my $bit;
		@bits = split (/\blong\b/, $line);
		$new="";
		foreach $bit (0 .. $#bits)
		{
		    $new=$new.$bits[$bit];
		    if ($bit != $#bits)
		    {
			# we're not the last bit, so we need to put in either "long" or "INT32"
			my $token="INT32";
			printf STDERR "$ilab:$inum token $bit, new='%s'\n",$new if ($verbose>3);
			if ( ( $new =~ /\b(how|so|bytes of the|entries in the|deg\. in a|too|such a|byte|bytes|rather a|very|Created:|elements|great|it.s a|go the|see|1k|\.one long|in the|bits|short or|short, or|as|to get the|digits|\. Another|To cut a|up of a single|characters|one element|which is BufferSize|fairly|entries|rename the|a power of|REM 10|don.t know the|if file has a|4 or 5|Sorry about the|characters are considered illegal when converting|Two dummy|Find the first bit of the|attribute node. from a|accomodate a) $/i ) ||
			     ( $bits[$bit+1] =~ /^.(story|winded|running|enough|grad table|, but|as possible|narrow boxes|while ago|it took me|life operation|ASCII string|before this|as that|last[,()]s+|time to|filename,\s+|\s+filename of|filename for|one. already exists|way away|names,s+|form, and|time to|filename version|ago|gone\. Returns|directory names|life time|time in the|way behind|pointer|time, there|mother of an extension|is . 32 bits on|and setting BIT_BUF_SIZE|\w+ = wx\w+)\b/i ) ||
			     ($bits[$bit] =~ /^s+$/)
			   )
			{
			    $token = "long";
			}
			printf STDERR "$ilab:$inum matched=$token\n" if ($verbose>3);
			$new=$new.$token;
		    }
		}
		$new=~s/\ba INT32\b/an INT32/g;
		$new=~s/\bA INT32\b/An INT32/g;
		if ($line ne $new)
		{
#		    printf STDERR "$ilab:$inum: FIXLONG1 %s",$line if ($verbose>3);
		    printf STDERR "$ilab:$inum: FIXLONG2 %s",$new if ($verbose>2);
		}
		$line = $new;
	    }
	}

	if ($fixnotice)
	{
	    print STDERR "$ilab:$inum: Matching against notice pattern $noticespat\n" if ($verbose>3);

	    if ($line =~ /$noticeskippat/)
	    {
		$preamble = 0;
		if ($buffering)
		{
		    # delete the inserted copyright notice from the
		    # buffer
		    my $sbuffpat;
		    $sbuffpat=makenpat($sbuff);
		    $buff =~ s/^$sbuffpat//s;
		    # dump out the buffer
		    print OUTPUT $buff;
		    $buff="";
		    $buffering=0;
		}
		printf STDERR "$ilab:$inum: Skipping copyright notice fix for third party code\n" if ($verbose>1);
	    }

	    if ($line =~ /$noticespat/)
	    {
		print STDERR "$ilab:$inum: Found notice\n" if ($verbose>1);

		# We've found the start of the copyright notice
		# Trash the old one by skipping to the end of it
		$line="";
		$origline=""; # Avoids it being marked as a change

		my $oldnotice="";

		while (<INPUT>)
		{
		    $inum++;
		    print STDERR "$ilab:$inum: Testing for end of skip: $_\n" if ($verbose>3);
		    print STDERR "$ilab:$inum: Matching against notice end pattern $noticeepat\n" if ($verbose>3);
		    last if /$noticeepat/;
		    $oldnotice=$oldnotice.$_;
		}

		# If the notice has changed, note this so the file gets written.
		if ($oldnotice ne $newnotice)
		{
		    printf STDERR "$ilab:$inum: Broken notice fixed\n" if ($verbose>1);
		    $changed = 1;
		}

		if ($verbose>2)
		{
		    printf STDERR "$ilab:$inum: >>>$oldnotice<<<\n>>>$newnotice<<<\nChanged: %d\n",
		    ($oldnotice ne $newnotice);;
		}

		$preamble = 0;

		# Only go further if we are buffering - else the notice is
		# duplicated and deleting it is a good thing.
		if ($buffering)
		{
		    # dump out the buffer
		    print OUTPUT $buff;
		    $buff="";
		    $buffering=0;
		}
		else
		{
		    # we deleted a duplicate notice - mark it as a change
		    $changed = 1;
		    printf STDERR "$ilab:$inum: Duplicate notice deleted\n" if ($verbose>1)
		}
	    }
	}

	# We allow through lines starting with // at the start of the file
	# even before our copyright notice. However we stick it in before the
	# first non-matching such line.
	$preamble = 0 if ($line !~ /^\/\//);
	
	if ( $origline ne $line)
	{
	    print STDERR "$ilab:$inum: Line changed\n" if (!$changed && $verbose>1);
	    $changed = 1;
	}
	
	if ($buffering && !$preamble)
	{
	    $buff=$buff.$line;
	}
	else
	{
	    print OUTPUT $line;
	}
    }

    print STDERR "$ilab:$inum: Buffering is $buffering\n" if ($verbose>2);

    if ($buffering)
    {
	# We found no copyright notice at all
	print OUTPUT $buff;
	$buff="";
	print STDERR "$ilab:$inum: No copyright message found, introducing one\n" if ($verbose>1);
	$changed=1;
    }
	
    printf STDERR "$ilab:$inum: File is %s\n",$changed?"changed":"unchanged" if ($verbose>0);

    close (INPUT);
    close (OUTPUT);

    return ($changed);
	
}

sub authorpat
{
    my $a=shift @_;
    return qr/^([ \t]*Author:[ \t]*)$a\b/;
}

# This makes precompiled regexps to make things faster
#
sub readstrings
{
    if ($fixauthors)
    {
	my %aname;

	open (AUTHORS, $authorfile) || die ("Can't read authors file $authorfile: $!");
	while (<AUTHORS>)
	{
	    chomp;
	    next if /^#/;
	    my $old;
	    my $name;
	    my $co;
	    my $email;
	    my $pat;

	    ($old, $name, $co, $email) = split(',');

	    # Use a pattern based on the old name
	    $pat = authorpat($old);
	    $authors{$pat}="$name ($co) <".$email.">";
	    $aname{$pat}=$old;

	    # Make another pattern entry out of the full name
	    $pat = authorpat($name);
	    $authors{$pat}="$name ($co) <".$email.">";
	    $aname{$pat}=$name;

	    # Make another pattern entry out of the full name with
	    # underscores coverted to spaces
	    my $n = $name;
	    $n =~ s/_/ /g;
	    $pat = authorpat($n);
	    $authors{$pat}="$name ($co) <".$email.">";
	    $aname{$pat}=$name;
	}

	# we cache the reverse sorted keys to avoid a quicksort per line
	# We have them sorted by reverse name, so we match (for instance)
	# "Tim Browse" before "Tim"
	#
	@kauthors = sort {$aname{$b} cmp $aname{$a}} keys %aname;

	close (AUTHORS);
    }

    if ($fixtypes)
    {

	%types = (
		  qr/\bSHORT\b/ => "INT8",
		  qr/\bUSHORT\b/ => "UINT8",
		  qr/\bPSHORT\b/ => "PINT8",
		  qr/\bPUSHORT\b/ => "PUINT8",
		  qr/\bLPSHORT\b/ => "PINT8",
		  qr/\bLPUSHORT\b/ => "PUINT8",
		  qr/\bLONG\b/ => "INT32",
		  qr/\bLONGs\b/ => "INT32s",
		  qr/\bLONGS\b/ => "INT32s",
		  qr/\bULONG\b/ => "UINT32",
		  qr/\bcLONG\b/ => "cINT32",    # Gavin in stroking code
		  qr/\bcULONG\b/ => "cUINT32",  # Gavin in stroking code
		  qr/\bcUINT\b/ => "cUINT32",   # Gavin in palette code
		  qr/\bPLONG\b/ => "PINT32",
		  qr/\bPULONG\b/ => "PUINT32",
		  qr/\bLPLONG\b/ => "PINT32",
		  qr/\bLPULONG\b/ => "PUINT32",
		  qr/\bLONGLONG\b/ => "INT64",
		  qr/\bULONGLONG\b/ => "UINT64",
		  qr/\bint\b/ => "INT32",
		  qr/\bunsigned int\b/ => "UINT32",
		  qr/\bunsigned long\b/ => "UINT32",
		  qr/\bINT\b/ => "INT32",
		  qr/\bUINT\b/ => "UINT32",
		  qr/\bPINT\b/ => "PINT32",
		  qr/\bINT_PTR\b/ => "PINT32",
		  qr/\bPUINT\b/ => "PUINT32",
		  qr/\bReadLONG\b/ => "ReadINT32",
		  qr/\bReadLONGnoError\b/ => "ReadINT32noError",
		  qr/\bReadULONG\b/ => "ReadUINT32",
		  qr/\bWriteLONG\b/ => "WriteINT32",
		  qr/\bWriteULONG\b/ => "WriteUINT32",
		  qr/\bLONG_MAX\b/ => "INT32_MAX",
		  qr/\bLONG_MIN\b/ => "INT32_MIN",
		  qr/\bULONG_MAX\b/ => "UINT32_MAX",
		  qr/\bULONG_MIN\b/ => "UINT32_MIN",
		  qr/\bBlendLONG\b/ => "BlendINT32",
		  qr/\bBlendULONG\b/ => "BlendUINT32",
		  qr/\bBlendUINT\b/ => "BlendUINT32",
		  qr/\bFTT_ULONG\b/ => "FTT_UINT32",
		  qr/\bFTT_LONG\b/ => "FTT_INT32",
		  qr/\bEPSTYPE_LONG\b/ => "EPSTYPE_INT32",
		  qr/\bEPSTYPE_ULONG\b/ => "EPSTYPE_UINT32",
		  qr/\bFloorAngleToLONG\b/ => "FloorAngleToINT32",
		  qr/\bFloorAngleFromLONG\b/ => "FloorAngleFromINT32",
		  qr/\bLONGFloorShadowAngle\b/ => "INT32FloorShadowAngle",
		  qr/\bGetTokenValLONG\b/ => "GetTokenValINT32",
		  qr/\bSwapLONG\b/ => "SwapINT32",
		  qr/\bSwapULONG\b/ => "SwapUINT32",
		  qr/\bMAKELONG\b/ => "MAKEINT32",
		  qr/\bSIGNED_LONG_ARG\b/ => "SIGNED_INT32_ARG",
		  qr/\bUNSIGNED_LONG_ARG\b/ => "UNSIGNED_INT32_ARG",
		  qr/\bFIXEDTOLONG\b/ => "FIXEDTOINT32",
		  qr/\bpULONG\b/ => "pUINT32",
		  qr/\bpLONG\b/ => "pINT32",
		  qr/\bPFNSTRINGTOLONG\b/ => "PFNSTRINGTOINT32"
		  );

	@ktypes = reverse sort keys %types;
    }

    if ($fixnotice)
    {
	open (NOTICE, $noticefile) || die ("Can't read copytight notice file $noticefile: $!");
	while (<NOTICE>)
	{
	    # make sure it has sane newlines
	    chomp;
	    s/\r//g;
	    $newnotice = $newnotice.$_."\n";
	}
	close (NOTICE);
	# Delete multiple returns at the end
	$newnotice =~ s/\n+$/\n/g;
    }

    if (scalar(@removeinclude))
    {
	my $i;
	my $p="";
	foreach $i (@removeinclude)
	{
	    $i=~s/\./\\\./g;
	    $p.="|" if ($p ne "");
	    $p.="($i)"
	}
#	$removeincludepat="^\\s*\\#include\\s+\\\"$p\\\".*\$";
	$removeincludepat=qr/^\s*\#include\s+\"($p)\".*$/;
	print STDERR "Remove pattern $removeincludepat\n" if ($verbose);
    }
}
