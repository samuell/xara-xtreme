#########################################################################
# LIBRARY SECTION
#########################################################################

# this is the XarLib library.
# it's name is used for all
# of the other Makefile.am variables
#noinst_LIBRARIES	= libXar.a

# Manually set the package include dir, else we install libraries
# in /usr/include/XaraLX which is not a good thing
pkgincludedir=$(includedir)/xarlib

# Main target
lib_LTLIBRARIES = libXar.la

# have to use the -I flags to point at the three directories concerned as I
# can't get this file to compile pretranslated source files.
XARLIBFLAGS = \
	-DUSE_NATIVE_XLONG -DBUILDSHADOWS -DOLD_MATRIX_TRANSFORMATIONS \
	-DVECTOR_STROKING -DEXCLUDE_FROM_XARLIB -DNEW_SHADOW_RENDER -DNO_XARACMS \
	-DNEW_FEATURES -DSHOWPORTNOTE -DDO_EXPORT \
	-Iinclude -I$(TOPDIR)/PreComp -I$(TOPDIR)/Kernel -I$(TOPDIR)/wxOil -I$(TOPDIR)/GDraw

# make sure this does NOT have our include files in the path
# xarlib needs to be first
libXar_la_CXXFLAGS = $(XARLIBFLAGS)

# Ensure these all begin with "../<directory>/" even if in the current directory
XARLIBCPP = \
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
	../xarlib/camelot.cpp \
	../xarlib/ensure.cpp \
	../xarlib/xarlib.cpp

# Ensure these all begin with "../<directory>/" even if in the current directory
XARLIBHEADERS = \
	../PreComp/camtypes.h \
	../wxOil/basestr.h \
	../wxOil/camresource.h \
	../wxOil/camstring.h \
	../wxOil/ccobject.h \
	../wxOil/compatdef.h \
	../wxOil/errors.h \
	../wxOil/exception.h \
	../wxOil/fixmem.h \
	../wxOil/fixst128.h \
	../wxOil/fixst256.h \
	../wxOil/fixstr16.h \
	../wxOil/fixstr32.h \
	../wxOil/fixstr64.h \
	../wxOil/fixstr8.h \
	../wxOil/handles.h \
	../wxOil/hardwaremanager.h \
	../wxOil/memblk.h \
	../wxOil/memdebug.h \
	../wxOil/cammemory.h \
	../wxOil/mfccontainer.h \
	../wxOil/pathname.h \
	../wxOil/stdwx.h \
	../wxOil/unicdman.h \
	../wxOil/varstr.h \
	../GDraw/gconsts.h \
	../Kernel/attr.h \
	../Kernel/ccfile.h \
	../Kernel/ccmaths.h \
	../Kernel/ccpanose.h \
	../Kernel/coord.h \
	../Kernel/cxfarrow.h \
	../Kernel/cxfcols.h \
	../Kernel/cxfdash.h \
	../Kernel/cxfdefs.h \
	../Kernel/cxfile.h \
	../Kernel/cxfmap.h \
	../Kernel/cxfrec.h \
	../Kernel/cxfrech.h \
	../Kernel/cxftags.h \
	../Kernel/doccoord.h \
	../Kernel/docrect.h \
	../Kernel/f16spec.h \
	../Kernel/fixed16.h \
	../Kernel/fixed.h \
	../Kernel/fttyplis.h \
	../Kernel/list.h \
	../Kernel/listitem.h \
	../Kernel/macros.h \
	../Kernel/matrix.h \
	../Kernel/pathproc.h \
	../Kernel/paths.h \
	../Kernel/pathtype.h \
	../Kernel/pathutil.h \
	../Kernel/rechcomp.h \
	../Kernel/taglists.h \
	../Kernel/vector.h \
	../Kernel/xlong.h \
	../Kernel/zdeflate.h \
	../Kernel/zinflate.h \
	../Kernel/zlib.h \
	../Kernel/zstream.h \
	../Kernel/zutil.h \
	../xarlib/camelot.h \
	../xarlib/ensure.h \
	../xarlib/xarlib.h

INSTHEADERS = $(addprefix include/xarlib/, $(notdir $(XARLIBHEADERS)) )
INSTCPP = $(notdir $(XARLIBCPP) )

libXar_la_SOURCES = $(XARLIBCPP) $(INSTHEADERS)

pkginclude_HEADERS = $(INSTHEADERS)

BUILT_SOURCES = $(INSTHEADERS)
#BUILT_SOURCES = $(INSTHEADERS) $(INSTCPP)

MOSTLYCLEANFILES = $(INSTHEADERS)
#MOSTLYCLEANFILES = $(INSTHEADERS) $(INSTCPP)

# Insert an additional dependency to get the headers and cpp files built
noinst_DATA = $(INSTHEADERS)
#noinst_DATA = $(INSTHEADERS) $(INSTCPP)

# Generate a directory of includes which has "xarlib/" in the include path
$(INSTHEADERS) : $(XARLIBHEADERS)
	@mkdir -p include/xarlib && \
	echo $(libXar_la_SOURCES) > /tmp/bar && \
	j=xarlib/$(filter %/$(notdir $@), $(XARLIBHEADERS) ) && \
	echo "Building xarlib header $@ from $(TOPDIR)/$$j" && \
	perl -pe 's,^[ \t]*#include[ \t]+\",#include \"xarlib/,' <$(TOPDIR)/$$j >$@ 

# Generate a directory of includes which has "xarlib/" in the include path
# We have to muck around with the prefixes here to avoid partial matches (coord.cpp matching doccoord.cpp etc.)
$(INSTCPP) : $(XARLIBCPP)
	@echo $(INSTCPP) > /tmp/foo && \
	j=$(filter %/$(notdir $@), $(addprefix xarlib/, $(XARLIBCPP)) ) && \
	echo "Building xarlib CPP file $@ from $(TOPDIR)/$$j" && \
	ls -la $(TOPDIR)/$$j && \
	#perl -pe 's,^[ \t]*#include[ \t]+\",#include \"xarlib/,' <$(TOPDIR)/$$j >$@ 

# Only build the static library until we have an API/ABI nailed down
libXar_la_LDFLAGS = -static

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = Xar.pc

bin_PROGRAMS = expandxar
expandxar_SOURCES = ExpandXar/expandxar.cpp
expandxar_LDADD = libXar.la $(WX_LIBS)
#expandxar_LDFLAGS = -all-static
expandxar_LDFLAGS = -static
expandxar_CXXFLAGS = $(XARLIBFLAGS)
