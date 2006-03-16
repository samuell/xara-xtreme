#!/bin/sh
TEMPFILE=/tmp/strings.xrc.$$
TEMPFILE2=/tmp/strings.source.$$
TEMPFILE3=/tmp/strings.windows.$$
Scripts/xml2flat.pl wxOil/xrc/strings.xrc | awk '{print $1}' | sort | uniq > $TEMPFILE
Scripts/cpp2flat.pl wxOil/Res/UKEnglish/stringtbl.cpp | sort | uniq > $TEMPFILE3
Scripts/all.sh | xargs cat | perl -ne '{chomp;while(s/.*\b_R\(([A-Z0-9a-z_]+)\)//) {print "$1\n";}}' | sort | uniq > $TEMPFILE2
diff -uw $TEMPFILE $TEMPFILE2 | egrep '^\+[A-Z][A-Z]' | cut -c2- | (while read f ; do egrep "\b$f\b" $TEMPFILE3 ; done) | perl -e 'print "// Missing resources found automatically\nSTRINGTABLE DISCARDABLE\nBEGIN\n"; while(<>) {s/\b_R\(([\w_]+)\)/\1/g;print "\t".$_}; print "END\n"'
rm -f $TEMPFILE $TEMPFILE2 $TEMPFILE3