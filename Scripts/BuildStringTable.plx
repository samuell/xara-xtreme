#!/usr/bin/perl
# BuildStringTable.plx
use warnings;
use strict;
use String::Strip;

my $fStringTable = 0;
my $fBegin = 0;
my $strLine;
my $strCurrent = "";
my $fNeedValue = 0;
my $strTag = "";
my $fInComment = 0;

my %hash;
my %hashInclude;

while( <> )
{
	if( $strCurrent ne $ARGV)
	{
		my $ordPeriod = rindex $ARGV, ".";
		my $ordSlash  = rindex $ARGV, "/";
		
		$strCurrent = substr $ARGV, $ordSlash + 1, $ordPeriod - $ordSlash - 1;
		$strCurrent = ucfirst $strCurrent
		
#		print "File = $strCurrent, $ordPeriod, $ordSlash\n";
	}
	
	$strLine = $_;
	StripLTSpace $strLine;
	
	if( 0 == index $strLine, "#include \"..\\" )
	{
		$hashInclude{$strLine} = 1;
	}
	
	if( $fBegin && 0 == index $strLine, "END"  )
	{
		$fBegin = 0;
		$fStringTable = 0;
	}

	if( $fBegin )
	{
		my $ord = index $strLine, "\"";
		
		if( $fInComment )
		{
			if( -1 != index $strLine, "*/" )
			{
				$fInComment = 0;
			}
			
#			print "ESkipping $strLine\n";
		}
		elsif( 0 == index $strLine, "/*" )
		{
			$fInComment = 1;

#			print "BSkipping $strLine\n";
		}
		elsif( 0 == index $strLine, "//" )
		{
#			print "Skipping $strLine\n";
		}
		elsif( $fNeedValue )
		{
#			print "Value = $strLine\n";
			
			$hash{$strCurrent}{$strTag} = $strLine;
			
			if( $strTag eq "" )
			{
#				print "Split \$strTag = NULL, $strLine\n";
			}
			
			$fNeedValue = 0;
		}
		elsif( -1 == $ord )
		{
#			print "Tag = ";
#			print index $strLine, "/*"; 
#			print ", \"$strLine\"\n";

			$fNeedValue = $strLine ne "";
			$strTag = $strLine;
		}
		else
		{
#			print "Tag\\Line = $strLine\n";
		
			my $ordSpace = index $strLine, " ";
			$ordSpace = -1 == $ordSpace ? 99999999 : $ordSpace;
			my $ordTab = index $strLine, "\t";
			$ordTab = -1 == $ordTab ? 99999999 : $ordTab;
			
			$strTag = substr $strLine, 0, $ordSpace < $ordTab ? $ordSpace : $ordTab;
			$strLine = substr $strLine, $ord;

			# Ignore TAG( stuff for moment
			if( 0 != index $strTag, "TIP(" )
			{
				$hash{$strCurrent}{$strTag} = $strLine;
			}
			else
			{
#				print "Is a TAG\n";
			}
		}
	}

	if( !$fBegin && 0 == index $strLine, "STRINGTABLE" )
	{
		$fStringTable = 1;
	}
	
	if( $fStringTable && 0 == index $strLine, "BEGIN" )
	{
		$fBegin = 1;
	}
}

print "#include \"camtypes.h\"\n";
print "\n";

for( keys %hashInclude )
{
	print "$_\n";
}

print "\n";
print "#include \"stringtbl.h\"\n";
print "\n";
print "std::map< UINT, PCTSTR >\tg_StringTable;\n";
print "\n";

for( keys %hash )
{
	print "static void Init$_()\n";
	print "{\n";

	my %hashInner = %{ $hash{$_} };
	for my $i ( keys %hashInner  )
	{
		print "\tg_StringTable[$i] = wxT($hashInner{$i});\n";
	}

	print "}\n\n";
}

print "void InitStringTable()\n";
print "{\n";
for( keys %hash )
{
	print "\tInit$_();\n";
}
print "}";
