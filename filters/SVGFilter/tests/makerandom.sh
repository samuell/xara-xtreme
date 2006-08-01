#!/bin/sh

for f in 1 2 3 4 5 6 7 8 9; do
    rm -f random$f.svg
    xemacs -vanilla -batch -l svg-generator.el -eval "(svg-generate-file $f \"random$f.svg\")"
done

exit 0
