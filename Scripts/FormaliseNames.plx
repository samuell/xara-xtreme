#!/usr/bin/perl
# BuildStringTable.plx
use warnings;
use strict;
use String::Strip;

my $strCurrent = "";
my $nLineCurrent = 0;
my $cTransEntry = 0;
my $strLine;

my %lines;
my %lookup;

sub Process
{
#	open FH, "> $strCurrent";

	my $ord;
	for( $ord = 0; $ord < $nLineCurrent; ++$ord)
	{
		if( $lines{$ord} =~ m/^([ \t]*Author:[ \t]*)(.*)$/i )
		{
			my ($name) = $2;
			my ($fTranslated) = 0;
			
			for( my ($ordTrans) = 0; $ordTrans < $cTransEntry; ++$ordTrans)
			{
				my ($strSearch) = "^$lookup{$ordTrans}{0}";
				if( $name =~ m/$strSearch/i )
				{
					print "Translated $name - > ";
					$name = "$lookup{$ordTrans}{1}, $lookup{$ordTrans}{2} ($lookup{$ordTrans}{3})";
					print "$name\n";

					$ordTrans = $cTransEntry;
					$fTranslated = 1;
				}
			}
			
			if( $fTranslated == 0 )
			{
				print "NOT translated $name\n";
			}
		
#			print "$1$name\n"
#			print "Found $2 - $ord\n";
		}
		else
		{
#			print FH $lines{$ord};
		}
	}
}

open FILE, "NameTable.txt";
while( <FILE> )
{
	chomp;
	$_ =~ m/(.*?),(.*?),(.*?),(.*?)$/;
	
#	print "$1|$2|$3|$4\n";
	$lookup{$cTransEntry}{0} = $1;
	$lookup{$cTransEntry}{1} = $2;
	$lookup{$cTransEntry}{2} = $3;
	$lookup{$cTransEntry}{3} = $4;

	++$cTransEntry
}

while( <> )
{
	if( $strCurrent ne $ARGV)
	{
		if( $strCurrent ne "")
		{
#			print "File = $strCurrent, $nLineCurrent\n";
			Process;
		}
		
		$strCurrent = $ARGV;
		$nLineCurrent = 0;
	}
	
	$lines{$nLineCurrent} = $_;
	++$nLineCurrent;
}

Process;
