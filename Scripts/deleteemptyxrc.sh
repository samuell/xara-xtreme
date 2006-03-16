#!/bin/sh

for i in wxOil/xrc/EN/*.xrc ; do
    if ( ! ( fgrep '</object>' >/dev/null $i ) ) ; then rm $i ; fi
done
       
