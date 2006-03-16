#!/bin/sh

find utils/input -name '*.rc' | while read f; do echo "Converting $f" 1>&2 ; Scripts/rc2xml.pl $f ; done
rm wxOil/xrc/EN/*.xrc
mv utils/input/*.xrc wxOil/xrc/EN

