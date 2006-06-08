#!/bin/sh
#
TMPFILE=/tmp/rem.$$
TMPFILE2=/tmp/rem2.$$
echo animparams.h app.h arrows.h attr.h attrmgr.h attrval.h barmsg.h bars.h basedoc.h basestr.h becomea.h biasgain.h binds.h binreloc.h bitmap.h bitmapcache.h bitmapcachekey.h bmplist.h camfiltr.h camresource.h camstring.h  capturemanager.h ccfile.h ccmaths.h ccobject.h clikmods.h colcarry.h colmodel.h compatdef.h convert.h coord.h cursor.h cxfdefs.h cxfrec.h cxfrech.h defalloc.h dialogop.h dibconv.h dibutil.h dlgtypes.h doccolor.h doccomp.h doccoord.h dockbar.h docmsgs.h docrect.h document.h docview.h docvmsg.h dragtrgt.h ensure.h epsclist.h epsfiltr.h epsstack.h errors.h exception.h fillattr.h fillval.h filters.h fix24.h fixed.h fixed16.h fixed24.h fixmem.h fixst128.h fixst256.h fixstr16.h fixstr32.h fixstr64.h fixstr8.h flags.h fontclass.h fraclist.h gconsts.h genlist.h genstack.h gradtype.h group.h handles.h ink.h iprofile.h iterator.h list.h listitem.h matrix.h memblk.h memdebug.h memory.h mfccontainer.h monotime.h msg.h node.h nodeattr.h nodecomp.h npaper.h oilcoord.h oilfltrs.h oilrect.h opdesc.h ops.h outptdib.h outptgif.h paldefs.h pathname.h paths.h pathshap.h pathtype.h prefs.h pump.h quality.h ralphint.h range.h rect.h release.h rendtype.h rgnlist.h rndrgn.h rndstack.h rrcaps.h selmsg.h selop.h selstate.h sgallery.h sgscan.h sgtree.h slowjob.h spread.h stdwx.h stockcol.h thumb.h tool.h tranform.h trans2d.h txtattr.h undoop.h units.h unittype.h varstr.h view.h wincoord.h winrect.h wrkcoord.h xlong.h xmatrix.h | perl -pe 's/\bcamtypes.h\b//g;' > $TMPFILE
# Scripts/gendeps.pl -t camtypes.h.gch debug/PreComp/.deps/*.Po | awk -F: '{print $2}') | perl -pe 's/\bcamtypes.h\b//g;' > $TMPFILE
( for i in `cat $TMPFILE` ; do echo --removeinclude $i ; done ) | xargs echo > $TMPFILE2


# Note do NOT process wxXtra or PreComp
DIRS="GDraw Kernel tools wxOil"
(cat $TMPFILE2 ; find $DIRS '(' -name '*.cpp' -or -name '*.c' -or -name '*.h' ')' -and -not -name camtypes.h -and -not -name `cat $TMPFILE | perl -pe 's/\s+$//g;s/^\s+//g;s/\s+/\ -and \-not \-name /g'` | egrep -v "wxOil/(xrc/)?(resources|winresources).cpp" ) | xargs Scripts/normalise.pl --no-fixauthors --no-fixtypes --fixnl --no-fixtrace --no-fixoptoken --no-fixresource --no-fixsvn --no-fixkeys --no-fixralph --no-fixnotice | tee /tmp/x
rm $TMPFILE
rm $TMPFILE2



