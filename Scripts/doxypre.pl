#!/usr/bin/perl

use strict;

my $inblock = 0;
my $skipheader = 0;

while (<>)
{
    chomp;
    if (s,/\*\*+,/\*\!,g)
    {
	$inblock=1;
	$skipheader=0;
    }
    $inblock=0 if s,\*+/,\*/,g;
    if ($inblock)
    {
	if (s/^[><].*$//g)
	{
	    # skip this till we get a blank line
	    $skipheader=1;
	}
	else
	{
	    # look for the blank line
	    $skipheader=0 if /^\s*$/;
	    $_="" if $skipheader;

	    s/^    /\t/;
	    s/^\s+Author:/\\author /;
	    s/^\s+Created:/\\date /;
	    s/^\s+Purpose:/\\brief /;
	    s/^\s+Inputs:(.*)$/\\param \1 [INPUTS]/;
	    s/^\s+Outputs:(.*)$/\\param \1 [OUTPUTS]/;
	    s/^\s+Returns:/\\return /;
	    s/^\s+Errors:/\\return Errors: /;
	    s/^\s+SeeAlso:/\\sa /;
	    
#	    s/^(\t\w+:)/\n\1/;
	}
    }

    print $_."\n";
}

