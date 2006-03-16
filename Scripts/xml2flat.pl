#!/usr/bin/perl -n
{
    chomp;
    if (/<object class="wxStaticText" name=\"(.*)\">/)
    {
	print "$1 ";
    }
    elsif (/<label>(.*)<\/label>/)
    {
	$line= "\"$1\"\n";
	$line=~s/Xara X\b/Xara Xtreme/g;
	print $line;
    }
}
