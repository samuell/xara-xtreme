#########################################################################
# LIBRARY SECTION
#########################################################################
# this is librairy that is part of main executable.  it's name is used for all
# of the other Makefile.am variables
#noinst_LIBRARIES	= libXar.a
lib_LTLIBRARIES = libXar.la

XARLIBFLAGS = \
	-DUSE_NATIVE_XLONG -DBUILDSHADOWS -DOLD_MATRIX_TRANSFORMATIONS \
	-DVECTOR_STROKING -DEXCLUDE_FROM_XARLIB -DNEW_SHADOW_RENDER -DNO_XARACMS \
	-DNEW_FEATURES -DSHOWPORTNOTE -DDO_EXPORT

# make sure this does NOT have our include files in the path
# xarlib needs to be first
libXar_la_CXXFLAGS = -I$(TOPDIR)/xarlib -I$(TOPDIR)/PreComp -I$(TOPDIR)/Kernel -I$(TOPDIR)/wxOil -I$(TOPDIR)/GDraw $(XARLIBFLAGS)

#Extra libraries
#libXar_la_LDFLAGS = @GTK_LIBS@

libXar_la_SOURCES = \
	../Kernel/ccfile.cpp \
	../Kernel/ccmaths.cpp \
	../Kernel/ccpanose.cpp \
	../Kernel/coord.cpp \
	../Kernel/cxfile.cpp \
	../Kernel/cxfmap.cpp \
	../Kernel/cxfrec.cpp \
	../Kernel/cxfrech.cpp \
	../Kernel/doccoord.cpp \
	../Kernel/docrect.cpp \
	../Kernel/f16spec.cpp \
	../Kernel/fixed16.cpp \
	../Kernel/fixed.cpp \
	../Kernel/fttyplis.cpp \
	../Kernel/list.cpp \
	../Kernel/listitem.cpp \
	../Kernel/matrix.cpp \
	../Kernel/pathproc.cpp \
	../Kernel/paths.cpp \
	../Kernel/pathutil.cpp \
	../Kernel/rechcomp.cpp \
	../Kernel/vector.cpp \
	../Kernel/xlong.cpp \
	../Kernel/zdeflate.cpp \
	../Kernel/zdftrees.cpp \
	../Kernel/ziftrees.cpp \
	../Kernel/zinflate.cpp \
	../Kernel/zstream.cpp \
	../wxOil/basestr.cpp \
	../wxOil/camresource.cpp \
	../wxOil/ccobject.cpp \
	../wxOil/errors.cpp \
	../wxOil/filehndl.cpp \
	../wxOil/filesize.cpp \
	../wxOil/fixmem.cpp \
	../wxOil/fixst128.cpp \
	../wxOil/fixst256.cpp \
	../wxOil/fixstr16.cpp \
	../wxOil/fixstr32.cpp \
	../wxOil/fixstr64.cpp \
	../wxOil/fixstr8.cpp \
	../wxOil/handles.cpp \
	../wxOil/makemsg.cpp \
	../wxOil/memblk.cpp \
	../wxOil/cammemory.cpp \
	../wxOil/pathname.cpp \
	../wxOil/unicdman.cpp \
	camelot.cpp \
	ensure.cpp \
	xarlib.cpp

if FALSE
pkginclude_HEADERS = PreComp/camtypes.h \
	wxOil/basestr.h \
	wxOil/camresource.h \
	wxOil/camstring.h \
	wxOil/ccobject.h \
	wxOil/compatdef.h \
	wxOil/errors.h \
	wxOil/exception.h \
	wxOil/fixmem.h \
	wxOil/fixst128.h \
	wxOil/fixst256.h \
	wxOil/fixstr16.h \
	wxOil/fixstr32.h \
	wxOil/fixstr64.h \
	wxOil/fixstr8.h \
	wxOil/handles.h \
	wxOil/hardwaremanager.h \
	wxOil/memblk.h \
	wxOil/memdebug.h \
	wxOil/memory.h \
	wxOil/mfccontainer.h \
	wxOil/pathname.h \
	wxOil/stdwx.h \
	wxOil/unicdman.h \
	wxOil/varstr.h \
	Kernel/attr.h \
	Kernel/ccfile.h \
	Kernel/ccmaths.h \
	Kernel/ccpanose.h \
	Kernel/coord.h \
	Kernel/cxfarrow.h \
	Kernel/cxfcols.h \
	Kernel/cxfdash.h \
	Kernel/cxfdefs.h \
	Kernel/cxfile.h \
	Kernel/cxfmap.h \
	Kernel/cxfrec.h \
	Kernel/cxfrech.h \
	Kernel/cxftags.h \
	Kernel/doccoord.h \
	Kernel/docrect.h \
	Kernel/f16spec.h \
	Kernel/fixed16.h \
	Kernel/fixed.h \
	Kernel/fttyplis.h \
	Kernel/list.h \
	Kernel/listitem.h \
	Kernel/macros.h \
	Kernel/matrix.h \
	Kernel/pathproc.h \
	Kernel/paths.h \
	Kernel/pathtype.h \
	Kernel/pathutil.h \
	Kernel/rechcomp.h \
	Kernel/taglists.h \
	Kernel/vector.h \
	Kernel/xlong.h \
	Kernel/zdeflate.h \
	Kernel/zinflate.h \
	Kernel/zlib.h \
	Kernel/zstream.h \
	Kernel/zutil.h \
	xarlib/camelot.h \
	xarlib/ensure.h \
	xarlib/gconsts.h \
	xarlib/xarlib.h

bin_PROGRAMS = expandxar

expandxar_SOURCES = ExpandXar/expandxar.cpp
expandxar_LDADD = libXar.la $(WX_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Xar.pc

endif
