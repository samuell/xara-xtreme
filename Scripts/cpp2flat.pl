#!/usr/bin/perl -n
{
#        g_StringTable[_R(IDS_COMBINE_ADD)] = wxT("&Add Shapes;Add shapes together to make their combined silhouette shape");
    chomp;
    if (/g_StringTable\[_R\((.*)\)\] = wxT\((.*)\);\s*$/)
    {
	$line="$1 $2\n";
	$line=~s/\&/&amp;/g;
	$line=~s/\</&lt;/g;
	$line=~s/\>/&gt;/g;
	$line=~s/Xara X\b/Xara Xtreme/g;
	print $line;
    }
}
