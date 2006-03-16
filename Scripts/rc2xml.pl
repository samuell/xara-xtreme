#!/usr/bin/perl

# (c) 2005 Alex Bligh / Xara Group Ltd
#
# Idea and code flow derived in part from covertrc.cpp in wxWindows
# distribution. Recoded in perl by Alex Bligh (alex@alex.org.uk)
# and adapted in several ways for Camelot.
#
# Original CPP program by Brian Gavin
#
# Issued under the wxWidgets license, though I'm happy to BSD
# or GPL it if Brian agrees.
#
# Note that this file is not required in order to build or use
# Camelot. It is merely a utility to convert the original WinOil
# resource files into .xrc format (and a hack at that). As only
# the /output/ is important, it is not required to be under the
# GPL. 

sub usage
{
    print STDERR <<EOF;

 rc2xml.pl -    a script to convert historic Camelot .rc files into their
                corresponding .xrc files

 Usage:

 rc2xml.pl [options] [<filename> ...]

 "-" may be specified as the file to read stdin. Not specifying a file will
 result in stdin being read.

 The script does not glob filenames, but if you are calling from a normal shell
 you can use shell globs on the command line, e.g.:
   normalise.pl Kernel/*.{cpp,h} wxOil/*.{cpp,h}

 The output filename will be equal to the input filename, with the following
 translations specified:
 * Conversion to lower case
 * Removal of suffixes of the form '.rc', '.rc2'
 * Addition of suffix '.xrc'

 The script will convert dialogs. String Tables are also converted into pseudo
 dialogs, which are dialogs build purely from static text controls with the
 a name starting "STRINGTABLE_". These are not designed for display, but as
 a temporary mechanism of getting text that isn't in dialogs into the Camelot
 XML resource file.

 Note there are three or four stages in preparation of resource files.
 * Conversion from Windows .rc format (hopefully happens infrequently)
 * Alteration of individual XML files (for instance, use an editor
   or DialogBlocks) - done by the developer. This happens in the
   wxOil/Res/${LANGUAGE} directory.
 * Compilation of a single XML file from the multiple XML files (possibly
   involving concatenation of STRINGTABLE resources) - done by 'make',
   which produces the "Res/" directory
 * Optional conversion of XML file and associated other binary files
   (for instance bitmaps) into a single .o file which is linked with
   the image.

 Options:
   --stdout                        - write the translated file to
                                     standard output (not the temp file)
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
my $ilab = 0;
my $inum = 0;
my $done = 0;
my $inputfile="";
my %stringtable;
my %bitmap;

GetOptions( "stdout!" => \$stdout,
	    "verbose|v+" => \$verbose,
	    "help!" => \$help ) || usage ("Bad option");

if ($help)
{
    usage();
}

# If no files specified, fake stdin as a file
push (@ARGV,"-") if ($#ARGV<0);

# Now iterate through each of the files specified on the command line

my $ifn;

foreach $ifn (@ARGV)
{
    my $ofn = lc($ifn);

    $ofn =~ s/\.rc([0-9])?$//;
    my $ofn2 = $ofn."-strings.xrc";
    $ofn = $ofn.".xrc";

#    my $output = do { local *INPUT };
#    my $input =  do { local *OUTPUT };
    my $tostdout = 0;

    if ($ifn eq "-")
    {
	$ifn="<&0";
	$ofn=">&1";
	$ofn2=">&1";
	$tostdout = 1;
    }

    if ($stdout)
    {
	$ofn=">&1";
	$ofn2=">&1";
	$tostdout = 1;
    }

    # process the file
    process ($ifn, $ofn, $ofn2, ($ifn eq "<&0")?"<STDIN>":$ifn);

}

exit (0);



sub LookUpString
{
    my $strid = shift @_;
    my $rst = shift @_;
    if (defined($stringtable{$strid}))
    {
	$$rst = $stringtable{$strid};
	return 1;
    }
    return 0;
}

sub LookUpBitmap
{
    my $strid = shift @_;
    my $rst = shift @_;
    if (defined($bitmap{$strid}))
    {
	$$rst = $bitmap{$strid};
	return 1;
    }
    return 0;
}

sub SplitHelp
{
    my $msg = shift @_;
    my $rshorthelp = shift @_;
    my $rlonghelp = shift @_;

    if ($msg !~ /\\n/)
    {
	$$rshorthelp =$msg;
	$$rlonghelp = $msg;
	return 0
    }

    ($$rshorthelp, $$rlonghelp) = split (/\\n/, $msg);

    return 1;
}

# Read a rectangle
sub ReadRect
{
    my $ret;
    my $rrect = shift @_;
    $$rrect[0] = GetToken ();
    $$rrect[1] = GetToken ();
    $$rrect[2] = GetToken ();
    $$rrect[3] = GetToken (\$ret);
    return $ret; # check for more parameters
}

sub RemapName
{
    my $name = shift @_;

    # Change wholly numeric names (which correspond to a windows resource ID that may
    # by load time have been assigned to something else) into a prefix
    $name=~s/^(\d+)$/ID_STATICDECIMAL_\1/;
   
    my %namemap = (
		   "IDOK" => "wxID_OK",
		   "IDCANCEL" => "wxID_CANCEL",
		   "IDAPPLY" => "wxID_APPLY",
		   "ID_FILE_OPEN" => "wxID_OPEN",
		   "ID_FILE_CLOSE" => "wxID_CLOSE",
		   "ID_FILE_SAVE" => "wxID_SAVE",
		   "ID_FILE_SAVE_AS" => "wxID_SAVEAS",
		   "ID_APP_EXIT" => "wxID_EXIT",
		   "ID_FILE_PRINT" => "wxID_PRINT",
		   "ID_FILE_PRINT_PREVIEW" => "wxID_PREVIEW",
		   "ID_FILE_PRINT_SETUP" => "wxID_PRINT_SETUP",
		   "ID_APP_ABOUT" => "wxID_ABOUT",
		   "ID_EDIT_UNDO" => "wxID_UNDO",
		   "ID_EDIT_CUT" => "wxID_CUT",
		   "ID_EDIT_COPY" => "wxID_COPY",
		   "ID_EDIT_PASTE" => "wxID_PASTE",
		   "IDYES" => "wxID_YES",
		   "IDNO" => "wxID_NO",
		   "IDHELP" => "wxID_HELP"
		   );

    return $namemap{ $name } || $name;
}

sub WriteName
{
    my $n=shift @_;
    my $name = RemapName($n);
    print OUTPUT " name=\"$name\"";
}

sub WriteLabel
{
    my $t=shift @_;
    $t=~s/</&lt;/g;
    $t=~s/>/&gt;/g;
    $t=~s/&/&amp;/g;
    print OUTPUT "\t\t\t<label>$t</label>\n";
}

sub WriteStyle
{
    my $t=shift @_;
    print OUTPUT "\t\t\t<style>$t</style>\n" unless ($t eq "");
}

sub WritePosition
{
    my $x = shift @_;
    my $y = shift @_;
    printf OUTPUT " <pos>%d,%dd</pos>",$x,$y;
}

sub WriteSize
{
    my $x = shift @_;
    my $y = shift @_;
    printf OUTPUT " <size>%d,%dd</size>",$x,$y;
}

sub WriteIcon
{
    my $t=shift @_;
    my $bname=lc($t).".png";
    $bname=$bitmap{$t} if defined($bitmap{$t});
    print OUTPUT "\t\t\t<bitmap>$bname</bitmap>\n";
}

sub WriteBitmap
{
    my $a=shift @_;
    my $b=shift @_;
    WriteIcon ($a, $b);
}

sub WriteTitle
{
    my $t=shift @_;
    $t=~s/</&lt;/g;
    $t=~s/>/&gt;/g;
    $t=~s/&/&amp;/g;
    print OUTPUT "\t\t<title>$t</title>\n";
}

# Write basic info
sub WriteBasicInfo
{
    my @rect;
    for (my $i=0; $i<=3; $i++) {$rect[$i]=shift @_};
    my $name=shift @_;
    WriteName($name);
    print OUTPUT ">\n\t\t\t";
    WritePosition($rect[0],$rect[1]);
    WriteSize($rect[2],$rect[3]);
    print OUTPUT "\n";
    
}

# Reads the next token. In order to do this, it skips separators
# which are blanks, commas and EOF
sub GetToken
{
    my $rsep = shift @_;
    # Drop leading separators
    $inputfile =~ s/^[,\s]+//;

    if ($inputfile eq "")
    {
	$done=1;
	return "";
    }

    $inputfile =~ s/^([^\s,]+)([\s,]+)//;
    my $ret = $1;
    if (defined ($rsep))
    {
	$$rsep = ( ($2 =~ /,/)?1:0 );
    }
    print STDERR "$ilab: TOKEN <$ret>\n" if ($verbose>3);
    return $ret;
}

# Reads the next token. In order to do this, it skips separators
# which are blanks, commas and EOF
sub PeekToken
{
    my $rsep = shift @_;

    # Drop leading separators
    $inputfile =~ s/^[,\s]+//;
    $inputfile =~ /^([^\s,]+)([\s,]+)/;
    my $ret = $1;
    if (defined ($rsep))
    {
	$$rsep = ( ($2 =~ /,/)?1:0 );
    }
    print STDERR "$ilab: PEEKTOKEN <$ret>\n" if ($verbose>3);
    return $ret;
}

sub p20
{
    printf STDERR "$ilab: %s P20<%s>\n",shift(@_),substr ((shift @_),0,79) if ($verbose>4);
}

# Reads the next token. In order to do this, it skips separators
# which are blanks, commas and EOF
sub GetStringQuote
{
    my $ret = "";
    # Drop leading separators
    $inputfile =~ s/^[,\s]+//;

    p20("1",$inputfile);

    my $q= '"';
    
    # Drop anything up to and including the leading quote
    $inputfile =~ s/^[^\"]*\"//;

    p20("2",$inputfile);

    my $again=0;

    do
    {
	$again=0;
	# take all the non-quote characters up to the close quote
	$inputfile =~ s/([^\"]*)\"//;
	$ret .= $1;

	p20("3",$inputfile);
	p20("4",$ret);

	# now it might have been a \" or a "" (both codes for a quote)
	# in both cases eat the characters concerned. Do the test separately
	# to cope with \"" (for instance). Note have to output a slashed
	# quote
	if ($ret =~ s/\\$/\"/)
	{
	    $again=1;
	    $ret .= '\\\"';
	}
	elsif ($inputfile =~ s/^$q//)
	{
	    $again=1;
	    $ret .= $q;
	}
    } while ($again);

    print STDERR "$ilab: GETSTRINGQUOTE <$ret>\n" if ($verbose>3);
    return $ret;
}

# These two seem to do much the same
sub GetQuoteField
{
    return GetStringQuote();
}

sub ReadOrs
{
    my $rorsstring = shift @_;
    my $token;

    $token = PeekToken();

    print STDERR "$ilab: ORSTOKENPEEK <$token>\n" if ($verbose>3);

    return 0 if ($token =~ /^\d+$/);

    $$rorsstring = GetToken();
    print STDERR "$ilab: ORSTRING1 <$$rorsstring>\n" if ($verbose>3);

    while (PeekToken () eq "|")
    {
	    $$rorsstring .= GetToken();
	    print STDERR "$ilab: ORSTRING2 <$$rorsstring>\n" if ($verbose>3);
	    $$rorsstring .= GetToken ();
	    print STDERR "$ilab: ORSTRING3 <$$rorsstring>\n" if ($verbose>3);
    }
    return 1;
}

# Read the input file, joining lines with a trailing backslash
# and omitting \r's
sub ReadInputFile
{
    while (<INPUT>)
    {
	s/\r//;
	chomp;
	my $line=$_;
	# put a return on if it ends in \\ or if it (otherwise) doesn't end in \ - in which case chop it
	$line = $line."\n" if ( ( /\\\\$/ ) || !( s/\\$//) );
	$inputfile .= $line;
    }
    $inputfile .= " "; # for good measure

    # Replace non-ascii characters with question marks
    $inputfile=~s/\P{IsASCII}/?/g;
    $inputfile=~s/\s+/ /g;
}


# LTEXT           "Radius",IDC_STATIC,9,67,23,8
sub ParseStaticText
{
    my $phrase = shift @_;
    my $varname = shift @_;
    my $token = PeekToken ();
    while ($token !~ /^\d+$/ )
    {
	GetToken ();
	$token = PeekToken ();
    }

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t\t<object class=\"wxStaticText\"";
    WriteBasicInfo (@rect, $varname);
    WriteLabel ($phrase);
    print OUTPUT "\t\t</object>\n";
}

# EDITTEXT        IDC_RADIUS,36,65,40,14,ES_AUTOHSCROLL
sub ParseTextCtrl
{
    my $varname = shift @_;
    my $token = PeekToken ();
    while ($token !~ /^\d+$/ )
    {
	GetToken ();
	$token = PeekToken ();
    }

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t\t<object class=\"wxTextCtrl\"";
    WriteBasicInfo (@rect, $varname);
    print OUTPUT "\t\t</object>\n";

}

# AUTOCHECKBOX "&log.", ID_XLOG, 25, 24, 21, 12
sub ParseCheckBox
{
    my $phrase = shift @_;
    my $varname = shift @_;
    my $token = PeekToken ();
    while ($token !~ /^\d+$/ )
    {
	GetToken ();
	$token = PeekToken ();
    }

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t\t<object class=\"wxCheckBox\"";
    WriteBasicInfo (@rect, $varname);
    WriteLabel ($phrase);
    print OUTPUT "\t\t</object>\n";
}

# AUTORADIOBUTTON "&text", ID_SW10, 13, 12, 68, 10, BS_AUTORADIOBUTTON | WS_GROUP
sub ParseRadioButton
{
    my $phrase = shift @_;
    my $varname = shift @_;
    my $token;
    my $style;
    my $GotOrs = ReadOrs (\$token);
    my @rect;
    if (ReadRect(\@rect))
    {
	ReadOrs (\$token) unless $GotOrs;
    }

    $style .= "wxRB_GROUP" if ($token =~ /WS_GROUP/);

    print OUTPUT "\t\t<object class=\"wxRadioButton\"";
    WriteBasicInfo (@rect, $varname);
    WriteLabel ($phrase);
    WriteStyle ($style);
    print OUTPUT "\t\t</object>\n";
}

# COMBOBOX        IDC_SCALECOMBO,10,110,48,52,CBS_DROPDOWNLIST | CBS_SORT |
#                    WS_VSCROLL | WS_TABSTOP 
sub ParseComboBox
{
    my $varname = shift @_;
    my $token;
    my $GotOrs = ReadOrs (\$token);
    my @rect;
    if (ReadRect(\@rect))
    {
	ReadOrs (\$token) unless $GotOrs;
    }

    print OUTPUT "\t\t<object class=\"wxComboBox\"";
    WriteBasicInfo (@rect, $varname);
    WriteStyle ("wxCB_SIMPLE") if ($token =~ /CBS_SIMPLE/);
    WriteStyle ("wxCB_SORT") if ($token =~ /CBS_SORT/);
    WriteStyle ("wxCB_ALWAYS_SB") if ($token =~ /CBS_DISABLENOSCROLL/);
    print OUTPUT "\t\t</object>\n";
}


#ICON            IDR_MAINFRAME,IDC_STATIC,11,17,20,20
sub ParseIconStatic
{
    my $token;
    my $varname;
    my $iconname;
    $token = PeekToken();
    if ($token =~ /\"/)
    {
	$iconname = GetQuoteField();
    }
    else
    {
	$iconname = GetToken();
    }

    $varname = GetToken ();

    my @rect;
    ReadRect (\@rect);
    print OUTPUT "\t\t<object class=\"wxStaticBitmap\"";
    WriteBasicInfo (@rect, $varname);
    WriteIcon ($iconname);
    print OUTPUT "\t\t</object>\n";
}

# //IDR_MAINFRAME           ICON    DISCARDABLE     "res\\mfcexample.ico"
sub ParseIcon
{
    my $token=PeekToken();
    my $iconfile=GetQuoteField();

    $bitmap{$token}=lc($iconfile);
    print STDERR "$ilab: ParseIcon found token=<$token> iconfile=<$iconfile>\n" if ($verbose >1);

}


sub ParsePushButton
{
    my $phrase = shift @_;
    my $varname = shift @_;
    my $token = PeekToken ();
    while ($token !~ /^\d+$/ )
    {
	GetToken ();
	$token = PeekToken ();
    }

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t\t<object class=\"wxButton\"";
    WriteBasicInfo (@rect, $varname);
    WriteLabel ($phrase);
    print OUTPUT "\t\t</object>\n";
}

# Self drawn controls
#    CONTROL         "Redraw Me",IDC_DIALOGDRAW1,"CC_DIALOGDRAW",
#                    WS_TABSTOP,4,5,120,119

sub ParseDialogDraw
{
    my $label = shift @_;
    my $varname = shift @_;
    my $token;
    my $style;

    ReadOrs (\$token);

    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxStaticBitmap\"";
    WriteBasicInfo (@rect, $varname);
    # <Bitmap></Bitmap> deliberately omitted
    print OUTPUT "\t\t</object>\n";

}


##    CONTROL         "BUTTONSTRIP.0",IDC_BTN_WINDOWARRANGE,"cc_BITMAPBUTTON",
##                    0x4,213,55,16,16

sub ParseCCBitmapButton
{
    my $label = shift @_;
    my $varname = shift @_;
    my $token;
    my $style;

    ReadOrs (\$token);

    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxCamArtControl\"";
    WriteBasicInfo (@rect, $varname);
# Don't write the label - it (correctly) causes a stringtable entry, which then (correctly)
# causes the app to look it up and use the specified bitmap, which of course doesn't exist
# with that name as it's a button strip. Instead, write a label of "-" which we then
# recognize for later
#    WriteLabel ($label); # default to a filename which is the label of the control
    WriteLabel ("-");
    print OUTPUT "\t\t</object>\n";

}

# And the award for most messed up control goes to...
#    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
sub ParseStaticBitmap
{
    my $bitmapname = shift @_;
    my $varname = shift @_;
    my $token;

    ReadOrs (\$token);

    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxStaticBitmap\"";
    WriteBasicInfo (@rect, $varname);
    WriteBitmap ($bitmapname);
    print OUTPUT "\t\t</object>\n";
}



sub ParseSlider
{
    my $label = shift @_;
    my $varname = shift @_;
    my $token;
    my $style="";
    ReadOrs (\$token);

    if ($token =~ /TBS_VERT/)
    {
	$style .= "wxSL_VERTICAL";
    }
    else
    {
	# MFC RC Default is horizontal
	$style .= "wxSL_HORIZONTAL";
    }

    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxSlider\"";
    WriteBasicInfo (@rect, $varname);
    WriteStyle ($style);
    print OUTPUT "\t\t</object>\n";
}

# SCROLLBAR       IDC_SCROLLBAR1,219,56,10,40,SBS_VERT
sub ParseScrollBar
{
    my $varname = GetToken ();

    my @rect;

    my $style="";
    my $token;

    if (ReadRect(\@rect))
    {
	ReadOrs (\$token);
    }

    if ($token=~/SBS_VERT/)
    {
	$style = "wxSB_VERTICAL";
    }
    else
    {
	$style = "wxSB_HORIZONTAL";
    }

    print OUTPUT "\t\t<object class=\"wxScrollBar\"";
    WriteBasicInfo (@rect, $varname);
    WriteStyle ($style);
    print OUTPUT "\t\t</object>\n";

}


#
# CONTROL         "Progress1",CG_IDC_PROGDLG_PROGRESS,"msctls_progress32",
#                    WS_BORDER,15,52,154,13
sub ParseProgressBar
{
    my $label = shift @_;
    my $varname = shift @_;
    my $token;
    my $style="";
    ReadOrs (\$token);

    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxGauge\"";
    WriteBasicInfo (@rect, $varname);
    WriteStyle ($style);
    print OUTPUT "\t\t</object>\n";

}

#    LISTBOX         IDC_LIST1,16,89,48,40,LBS_SORT | LBS_MULTIPLESEL |
#                    LBS_NOINTEGRALHEIGHT | WS_VSCROLL | WS_TABSTOP
sub ParseListBox
{
    my $varname= shift @_;
    my $token = PeekToken ();
    while ($token !~ /^\d+$/)
    {
	$token = GetToken ();
	$token = PeekToken ();
    }

    $token="";
    my @rect;
    if (ReadRect(\@rect))
    {
	ReadOrs (\$token);
    }

    print OUTPUT "\t\t<object class=\"wxListBox\"";
    WriteBasicInfo (@rect, $varname);
    WriteStyle("wxLB_SORT") if ($token=~/\bLBS_SORT\b/);
    print OUTPUT "\t\t</object>\n";
}

# CONTROL         "Spin1",IDC_SPIN1,"msctls_updown32",UDS_ARROWKEYS,209,72,
#                 19,26
sub ParseSpinCtrl
{
    my $label = shift @_;
    my $varname = shift @_;
    my $token;
    my $style;

    ReadOrs (\$token);
    if ($token =~ /UDS_HORZ/)
    {
	$style = "wxSP_HORIZONTAL";
    }
    else
    {
	# MFC default
	$style = "wxSP_VERTICAL";
    }


    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxSpinButton\"";
    WriteBasicInfo (@rect, $varname);
    WriteStyle ($style);
    print OUTPUT "\t\t</object>\n";

}

#    CONTROL         "List1",IDC_BARLIST,"cc_CustomList",WS_BORDER |
#                    WS_TABSTOP,4,4,125,62
sub ParseCustomListBox
{
    my $label = shift @_;
    my $varname = shift @_;
    my $token;
    my $style;

    ReadOrs (\$token);

    my @rect;
    ReadRect (\@rect);

    print OUTPUT "\t\t<object class=\"wxListBox\""; #Bodge
    WriteBasicInfo (@rect, $varname);
    WriteStyle ($style);
    print OUTPUT "\t\t</object>\n";

}

#    CONTROL         "Tree1",IDC_TREE1,"SysTreeView32",WS_BORDER | WS_TABSTOP,
#                      7,7,66,61

sub ParseTreeCtrl
{
    my $label = shift @_; # unused
    my $varname = shift @_;

    my $token = "";

    ReadOrs(\$token);

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t\t<object class=\"wxTreeCtrl\"";
    WriteBasicInfo (@rect, $varname);
    print OUTPUT "\t\t</object>\n";
}


##    GROUPBOX        "Rotate",IDC_STATIC,1,1,71,79
sub ParseGroupBox
{
    my $phrase = shift @_;
    my $varname = shift @_;
    my $token = PeekToken ();
    while ($token !~ /^\d+$/ )
    {
	GetToken ();
	$token = PeekToken ();
    }

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t\t<object class=\"wxStaticBox\"";
    WriteBasicInfo (@rect, $varname);
    WriteLabel ($phrase);
    print OUTPUT "\t\t</object>\n";
}

# Is it a checkbutton or a radiobutton or a pushbutton or a groupbox
sub ParseCtrlButton
{
    my $label = shift @_;
    my $varname = shift @_;

    my $token;
    my $ii = $inputfile;
    ReadOrs (\$token);
    $inputfile = $ii;

    if ($token =~ /BS_AUTOCHECKBOX/)
    {
	ParseCheckBox($label, $varname);
    }
    elsif ($token =~ /BS_(AUTO)?RADIOBUTTON/)
    {
	ParseRadioButton($label, $varname);
    }
    elsif ($token =~ /BS_GROUPBOX/)
    {
	ParseGroupBox($label, $varname);
    }
    else # if ($token =~ /BS_(DEF)?PUSHBUTTON/)
    {
	ParsePushButton($label, $varname);
    }
}


sub ParseControls ()
{
    my $token;
    my $label;
    my $varname;

    $token = GetToken ();
    while (($token ne 'END') && ($token ne '}'))
    {
	if ($token eq "AUTOCHECKBOX")
	{
	    $label = GetQuoteField ();
	    $varname = GetToken ();
	    ParseCheckBox ($label, $varname);
	}
	elsif ($token eq "AUTORADIOBUTTON")
	{
	    $label = GetQuoteField ();
	    $varname = GetToken ();
	    ParseRadioButton ($label, $varname);
	}
	elsif ($token eq "LTEXT")
	{
	    $label = GetQuoteField ();
	    $varname = GetToken ();
	    ParseStaticText ($label, $varname);
	}
	elsif ($token eq "EDITTEXT")
	{
	    $varname = GetToken ();
	    ParseTextCtrl ($varname);
	}
	elsif (($token eq "PUSHBUTTON") || ($token eq "DEFPUSHBUTTON"))
	{
	    $label = GetQuoteField ();
	    $varname = GetToken ();
	    ParsePushButton ($label, $varname);
	}
	elsif ($token eq "GROUPBOX")
	{
	    $label = GetQuoteField ();
	    $varname = GetToken ();
	    ParseGroupBox ($label, $varname);
	}
	elsif ($token eq "COMBOBOX")
	{
	    $varname = GetToken ();
	    ParseComboBox ($varname);
	}
	elsif ($token eq "CONTROL")
	{
	    ParseControlMS ();
	}
	elsif ($token eq "LISTBOX")
	{
	    $varname = GetToken ();
	    ParseListBox ($varname);
	}
	elsif ($token eq "ICON")
	{
	    ParseIconStatic ();
	}
	elsif ($token eq "SCROLLBAR")
	{
	    ParseScrollBar ();
	}

	$token = GetToken ();
    }

}


sub ParseNormalMSControl
{
    my $label = GetQuoteField();
    my $varname = GetToken();
    my $kindctrl = uc(GetQuoteField());

    if ($kindctrl eq "MSCTLS_UPDOWN32")
    {
	ParseSpinCtrl ($label, $varname);
    }
    elsif ($kindctrl eq "MSCTLS_TRACKBAR32")
    {
	ParseSlider ($label, $varname);
    }
    elsif ($kindctrl eq "MSCTLS_PROGRESS32")
    {
	ParseProgressBar ($label, $varname);
    }
    elsif ($kindctrl eq "SYSTREEVIEW32")
    {
	ParseTreeCtrl ($label, $varname);
    }
    elsif ($kindctrl eq "SYSMONTHCAL32")
    {
	ParseCalendar ($label, $varname);
    }
    elsif ($kindctrl eq "SYSLISTVIEW32")
    {
	ParseListCtrl ($label, $varname);
    }
    elsif ($kindctrl eq "BUTTON")
    {
	ParseCtrlButton ($label, $varname);
    }
    elsif ($kindctrl eq "RICHEDIT")
    {
	ParseRichEdit ($label, $varname);
    }
    elsif ($kindctrl eq "STATIC")
    {
	my $token;
	my $ii=$inputfile;
	ReadOrs (\$token);
	$inputfile = $ii;
	if ($token =~ /SS_BITMAP/)
	{
	    ParseStaticBitmap ($label, $varname);
	}
	else
	{
	    ParseStaticText ($label, $varname);
	}
    }
    elsif ($kindctrl eq "EDIT")
    {
	ParseTextCtrl ($varname);
    }
    elsif ($kindctrl eq "LISTBOX")
    {
	ParseListBox ($varname);
    }
    elsif ($kindctrl eq "COMBOBOX")
    {
	ParseComboBox ($varname);
    }
    elsif ($kindctrl eq "CC_SLIDER")
    {
	ParseSlider ($label, $varname);
    }
    elsif ($kindctrl eq "CC_DIALOGDRAW")
    {
	ParseDialogDraw ($label, $varname);
    }
    elsif ($kindctrl eq "CC_COLPICKER")
    {
	ParseDialogDraw ($label, $varname);
    }
    elsif ($kindctrl eq "OCTREE")
    {
	ParseDialogDraw ($label, $varname);
    }
    elsif ($kindctrl eq "CC_GRID")
    {
	ParseDialogDraw ($label, $varname);
    }
    elsif ($kindctrl eq "CC_BITMAPBUTTON")
    {
	ParseCCBitmapButton ($label, $varname);
    }
    elsif ($kindctrl eq "CC_TEXT3D")
    {
	ParseStaticText ($label, $varname);
    }
    elsif ($kindctrl eq "CC_SMALLBUTTON")
    {
	ParseCCBitmapButton ($label, $varname);
    }
    elsif ($kindctrl eq "CC_2DBITMAPCOMBOBOXEDIT")
    {
	ParseComboBox ($varname); # a bodge
    }
    elsif ($kindctrl eq "COMBOBOXEX32")
    {
	ParseComboBox ($varname); # a bodge
    }
    elsif ($kindctrl eq "CC_CUSTOMEDIT")
    {
	ParseTextCtrl ($varname); # a bodge
    }
    elsif ($kindctrl eq "CC_CUSTOMLIST")
    {
	ParseCustomListBox ($label, $varname);
    }
    elsif ($kindctrl eq "CC_1DBITMAPCOMBOBOXEDIT")
    {
	ParseComboBox ($varname); # a bodge
    }
    elsif ($kindctrl eq "CC_STATICBITMAP")
    {
	ParseStaticBitmap ($label, $varname);
    }
    else
    {
	print STDERR "$ilab: Warning: Did not understand label=<$label> varname=<$varname> kindctrl=<$kindctrl>\n" if ($verbose >0);
    }
}

sub ParseWeirdMSControl
{
    my $id = GetToken();
    my $varname = GetToken();
    my $kindctrl = uc(GetQuoteField());
#    CONTROL         IDB_FACE,IDC_STATIC,"Static",SS_BITMAP,26,62,32,30
    if ($kindctrl eq "STATIC")
    {
	ParseStaticBitmap ($id, $varname) if (PeekToken () eq "SS_BITMAP");
    }
    else
    {
	print STDERR "Unknown MS Control Static token\n" if ($verbose > 0);
    }
}

sub ParseBitmap
{
    my $varname = shift @_;
    my $token=PeekToken();

    if ($token eq "DISCARDABLE")
    {
	$token = GetToken ();
	$token = PeekToken ();
    }
    my $bitmapfile = GetQuoteField ();

    # Allegedly we should remember it here
    $bitmap{$varname}=lc($bitmapfile);
    print STDERR "$ilab: ParseBitmap found varname=<$varname> bitmapfile=<$bitmapfile>\n" if ($verbose > 1);
}

sub ParseControlMS
{
    my $token = PeekToken();

    if ($token =~ /\"/)
    {
	ParseNormalMSControl ();
    }
    else
    {
	ParseWeirdMSControl ();
    }
}

sub ParseDialog
{
    my $dlgname = shift @_;
    my $token = PeekToken();
    my $title="";
    my $fontsize=0;
    my $font=""; # we do nothing with this

    while ( $token =~ /^(DISCARDABLE)|(LOADONCALL)|(MOVEABLE)$/ )
    {
	GetToken();
	$token = PeekToken();
    }

    return unless $token=~/^\d+/;

    # read position
    my @rect;
    ReadRect(\@rect);

    print OUTPUT "\t<object class=\"wxDialog\"";

    WriteBasicInfo (@rect, $dlgname);

    while (($token ne 'BEGIN') && ($token ne '{'))
    {
	$title = GetQuoteField() if ($token eq 'CAPTION');
	if ($token eq 'FONT')
	{
	    $fontsize = GetToken();
	    $font = GetQuoteField() ;
	}
	$token = GetToken ();
    }

    ParseControls();
    WriteTitle ($title);
    print OUTPUT "\t</object>\n";
}

#
# We write the string table out as a dummy dialog
#
sub WriteStringTable
{
    return if (!defined(%stringtable));
    my $name = uc($ilab);
    # remove vestiges of directory path
    $name =~ s/.*[\\\/]([^\\\/]*)/\1/g;
    # remove non alphanumerics
    $name =~ s/[^A-Z0-9]//g;
    print OUTPUT "\t<object class=\"wxDialog\"";
    WriteName("STRINGTABLE_".$name);
    print OUTPUT ">\n";
    WriteTitle("String table from $name");
    print OUTPUT "\t\t<object class=\"wxBoxSizer\">\n";
    print OUTPUT "\t\t\t<orient>wxVERTICAL</orient>\n";

    my $key;

    foreach $key (sort keys %stringtable)
    {
	# Avoid some windows spuriousness
	if ($key=~/^[a-zA-Z0-9_]+$/)
	{
	    print OUTPUT "\t\t\t<object class=\"sizeritem\">\n";
	    print OUTPUT "\t\t\t\t<flag>wxALIGN_LEFT|wxALL|wxADJUST_MINSIZE</flag>\n";
	    printf OUTPUT "\t\t\t\t<object class=\"wxStaticText\" name=\"%s\">\n", RemapName($key);
	    print OUTPUT "\t\t";
	    WriteLabel($stringtable{$key});
	    print  OUTPUT "\t\t\t\t</object>\n";
	    print  OUTPUT "\t\t\t</object>\n";
	}
    }
    print OUTPUT "\t\t</object>\n";
    print OUTPUT "\t</object>\n";
}


sub ParseMenuItem
{
    my $token;
    my $name;
    my $msg;
    my $tip;
    my $longhelp;
    
    if (PeekToken() eq "SEPARATOR")
    {
	print OUTPUT "\t\t\t<object class=\"separator\"/>\n";
	return;
    }

    $token = GetQuoteField ();
    $name = GetToken ();

    print OUTPUT "\t\t\t<object class=\"wxMenuItem\"";
    WriteName($name);
    print OUTPUT ">\n";
    WriteLabel($token);

    if (($name !~ /^\d+$/ ) && (LookUpString ($name, \$msg)))
    {
	SplitHelp ($msg, \$tip, \$longhelp);
	print OUTPUT "\t\t\t<help>$longhelp</help>\n";
    }
    
# look for extra attributes like checked and break

    my $ptoken = uc(PeekToken ());
    while (($ptoken ne "MENUITEM") && ($ptoken ne "POPUP")
	   && ($ptoken ne "END"))
    {
	$token = uc(GetToken ());
	if ($token eq "CHECKED")
	{
	    print OUTPUT "\t\t\t<checkable>1</checkable>\n";
	}
	elsif ($token eq "MENUBREAK")
	{
	    #print OUTPUT "\t\t\t</break>\n";
	}
	elsif ($token eq "GRAYED")
	{
	}
	else
	{
	    print STDERR "$ilab: Unknown Menu Item token: <$token>" if ($verbose > 0);
	}

	$ptoken = uc(PeekToken ());
    }
    print OUTPUT "\t\t\t</object>\n";
    
}



sub ParsePopupMenu
{
    my $name=shift @_;
    my $token;
    my $msg;
    my $longhelp;
    my $tip;
    my $popupcount=0;
    
    $token = GetQuoteField ();
    
    print OUTPUT "\t\t<object class=\"wxMenu\"";
    WriteName ($name);
    print OUTPUT ">\n";
    WriteLabel($token);

    while (($token ne "BEGIN") && ($token ne '{'))
    {
	$token = GetToken ();
    }

    while (($token ne 'END') && ($token ne '}'))
    {
	$token = uc(GetToken ());
	
	if ($token eq "POPUP")
	{
	    $popupcount += 1;
	    ParsePopupMenu($name."_POPUP".$popupcount);
	}

	if ($token eq "MENUITEM")
	{
	    ParseMenuItem ();
	}
    }
    print OUTPUT "\t\t\t</object>\n";
}



sub ParseMenu
{
    my $varname = shift @_;
    my $token = "";
    my $popupcount=0;

    print OUTPUT "\t<object class=\"wxMenuBar\"";
    #	varname.Replace (_T ("IDR_"), _T ("MB_"));
    WriteName ($varname);
    print OUTPUT ">\n";

    while (($token ne "BEGIN") && ($token ne '{'))
    {
	$token = GetToken();
    }

    while (($token ne ("END")) && ($token ne '}'))
    {
	$token = uc(GetToken());
	if ($token eq "POPUP")
	{
	    $popupcount += 1;
	    ParsePopupMenu($varname."_POPUP".$popupcount);
	}
    }
   print OUTPUT "\t</object>\n";
}

sub ParseStringTable
{
    my $token = GetToken ();
    while (($token ne "BEGIN") && ($token ne '{'))
    {
	$token = GetToken ();
    }
    $token = GetToken ();

    while (($token ne "END") && ($token ne '}'))
    {
	$stringtable{$token}=GetStringQuote ();
	$token = GetToken ();
    }
}

sub process
{
    my $in = shift @_;
    my $out = shift @_;
    my $out2 = shift @_;

    undef %stringtable;
    undef %bitmap;

    $ilab = shift @_;
    $inum=0;
    $done=0;
    $inputfile="";

    my $line;
    my $buff="";
    my $prevtoken;


    print STDERR "$ilab: Starting processing\n" if ($verbose>1);

    open (INPUT, $in) || die ("Cannot open input file: $!");
    ReadInputFile();
    close (INPUT);

    my $ikeep = $inputfile;

    # First pass
    $prevtoken = "";
    do
    {
	my $token =GetToken();
	
	ParseBitmap($prevtoken) if ($token eq "BITMAP");
	ParseStringTable($prevtoken) if ($token eq "STRINGTABLE");
	ParseIcon($prevtoken) if ($token eq "ICON");
	if (($token eq "DIALOG") || ($token eq "MENU") || ($token eq "TOOLBAR"))
	{
	    # we want to skip the table entirely.
	    do
	    {
		$token =GetToken();
	    } while (!$done && ($token ne "END"));
	}

	$prevtoken=$token;

    } while (!$done);

    $inputfile = $ikeep;
    
    open (OUTPUT, ">$out") || die ("Cannot open ouput file $out: $!");

    print OUTPUT "<?xml version=\"1.0\" ?>\n<resource>\n";

    
    # Second pass
    $done = 0;
    $prevtoken = "";
    do
    {
	my $token =GetToken();
	
	ParseDialog($prevtoken) if (($token eq "DIALOG") || ($token eq "DIALOGEX"));
	ParseMenu($prevtoken) if ($token eq "MENU");
	ParseToolbar($prevtoken) if ($token eq "TOOLBAR");
	
	$prevtoken=$token;

    } while (!$done);

    print STDERR "$ilab: Writing string table\n" if ($verbose>1);

    print OUTPUT "</resource>\n";

    close (OUTPUT);

    open (OUTPUT, ">$out2") || die ("Cannot open string table file $out2: $!");

    print OUTPUT "<?xml version=\"1.0\" ?>\n<resource>\n";

    WriteStringTable();

    print OUTPUT "</resource>\n";

    close (OUTPUT);
    
    print STDERR "$ilab: Done processing\n" if ($verbose>1);


    return;
	
}
