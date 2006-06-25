#########################################################################
# LIBRARY SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
BUILT_SOURCES	= svnversion.h resources.h
# Note we don't clean xrc.d and xrc.stamp here to avoid a loop
CLEANFILES		= svnversion.h
# explicitly remove resources.h from TOPDIR as well. It should not be there on an out of tree build,
# but running build-resources manually (or switching to an out-of-tree build) can leave it there
# which then confuses things
MOSTLYCLEANFILES = $(TOPDIR)/wxOil/resources.h resources.h xrc/dialogs.xrc xrc/strings.xrc xrc/strings.lst xrc/xaralx.po xrc/resources.xrs xrc/xrc.check
noinst_LIBRARIES	= libwxOil.a

# the application source, library search path, and link libraries
libwxOil_a_SOURCES = \
	varstr.cpp \
	basestr.cpp camdoc.cpp camelot.cpp camframe.cpp \
	camview.cpp ccobject.cpp ccdc.cpp clikmods.cpp convert.cpp \
	diagnost.cpp dibconv.cpp dibutil.cpp dlgmgr.cpp dlgview.cpp ensure.cpp \
	errors.cpp filehndl.cpp filesize.cpp fixmem.cpp fixstr8.cpp fixstr16.cpp \
	fixstr32.cpp fixstr64.cpp fixst128.cpp fixst256.cpp handles.cpp makemsg.cpp \
	memblk.cpp memory.cpp monotime.cpp mtrand.cpp oilbitmap.cpp oilcoord.cpp \
	oildbug.cpp oilrect.cpp pathname.cpp pathnmex.cpp pen.cpp sgliboil.cpp \
	tunemem.cpp unicdman.cpp vstate.cpp wincoord.cpp oilfiles.cpp oilmenus.cpp \
	gifutil.cpp giffiltr.cpp winrect.cpp grndrgn.cpp osrndrgn.cpp grndbmp.cpp rendbits.cpp \
	palman.cpp gdrawcon.cpp fuzzclip.cpp cursor.cpp pngfiltr.cpp pngutil.cpp \
	maskfilt.cpp oilfltrs.cpp speedtst.cpp gbrush.cpp grnddib.cpp grndclik.cpp \
	offscrn.cpp camprofile.cpp localenv.cpp camresource.cpp oilmods.cpp \
	progress.cpp dockbar.cpp basebar2.cpp keypress.cpp sgindgen.cpp \
	dlgevt.cpp cartprov.cpp cartctl.cpp colourmat.cpp menucmds.cpp menupref.cpp \
	rendwnd.cpp scroller.cpp helpuser.cpp oilprefs.cpp dragtrgt.cpp \
	ctrllist.cpp dlgcol.cpp thumb.cpp dragmgr.cpp ccolbar.cpp dragcol.cpp \
	drawctl.cpp filedlgs.cpp fileutil.cpp ktimer.cpp camplatform.cpp \
	outptdib.cpp outptpng.cpp outptgif.cpp gpalopt.cpp bmpfiltr.cpp giffiltr.cpp \
	fontbase.cpp ftfonts.cpp textfuns.cpp dragbmp.cpp xpoilflt.cpp xmlutils.cpp \
	camprocess.cpp dropdown.cpp coldrop.cpp fontdrop.cpp bfxalu.cpp bfxpixop.cpp \
	binreloc.c exceptio.cpp colpick.cpp dragpick.cpp sgldrag.cpp cctime.cpp \
	lddirect.cpp prncamvw.cpp prdlgctl.cpp psdc.cpp grndprnt.cpp printprg.cpp \
	resources.cpp

# Keep resources.cpp on a separate line as it is odd

AM_CXXFLAGS = $(XARAFLAGS)

XARALANGUAGE = @XARALANGUAGE@

# Ensure wxrc is set to something. If wxconfig can't find it, it becomes an empty string which will upset build-resources.pl
WXRC := ${if @WXRC@,@WXRC@,echo}

RESOURCEDUMMY := ${if ${strip ${shell $(TOPDIR)/Scripts/build-resources.pl @BUILDRESFLAGS@ -t $(TOPDIR) -o . --version $(VERSION) -x $(XARALANGUAGE) -u $(USER) --wxrc "$(WXRC)" } }, ,${error "Resource build failed"}}

