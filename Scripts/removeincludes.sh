#!/bin/sh
#
TMPFILE=/tmp/rem.$$
TMPFILE2=/tmp/rem2.$$
( for i in `(Scripts/gendeps.pl -t camtypes.h.gch debug/PreComp/.deps/*.Po | awk -F: '{print $2}') | tee $TMPFILE `  ; do echo --removeinclude $i ; done ) > $TMPFILE2

DIRS="GDraw Kernel PreComp tools wxOil wxXtra"
(cat $TMPFILE2 ; find $DIRS '(' -name '*.cpp' -or -name '*.c' -or -name '*.h' ')'  `cat $TMPFILE | perl -pe 's/\s+$//g;s/\s+/\ -and \-not \-name /g'` | egrep -v "wxOil/(xrc/)?(resources|winresources).cpp" ) | xargs Scripts/normalise.pl
rm $TMPFILE
rm $TMPFILE2



