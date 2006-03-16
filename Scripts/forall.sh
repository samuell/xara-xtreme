#!/bin/sh
#
# This runs the command line on all files

DIRS="GDraw Kernel PreComp tools wxOil wxXtra"
find $DIRS -name '*.cpp' -or -name '*.c' -or -name '*.h' | xargs $*
