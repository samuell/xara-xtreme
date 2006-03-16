#!/bin/sh

fgrep '<bitmap>' wxOil/xrc/dialogs.xrc | sed -e 's,^.*<bitmap>,,;s,<\/bitmap>.*$,,' | while read f ; do
	if ( ! [[ -a wxOil/xrc/$f ]] ) ; then echo Missing: $f ; fi
done
