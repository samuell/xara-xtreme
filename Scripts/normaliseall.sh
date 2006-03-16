#!/bin/sh
#
# This runs normalise on all files

DIRS="GDraw Kernel PreComp tools wxOil wxXtra"
find $DIRS -name '*.cpp' -or -name '*.c' -or -name '*.h' | egrep -v "wxOil/(xrc/)?(resources|winresources).cpp" | xargs Scripts/normalise.pl $*
