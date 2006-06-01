export XARAFLAGS = -I../PreComp -I../wxOil -I$(TOPDIR)/Kernel -I$(TOPDIR)/wxOil -I$(TOPDIR)/wxOil/Res \
	-I$(TOPDIR)/tools -I$(TOPDIR)/GDraw -I$(TOPDIR)/PreComp -I$(TOPDIR)/wxXtra \
	-DUSE_NATIVE_XLONG -DBUILDSHADOWS -DOLD_MATRIX_TRANSFORMATIONS \
	-DVECTOR_STROKING -DEXCLUDE_FROM_XARALX -DNEW_SHADOW_RENDER -DNO_XARACMS \
	-DNEW_FEATURES -DSHOWPORTNOTE -DDO_EXPORT
export TOPDIR

PRECOMPILE_DIR =
if PRECOMPILE_ENABLE
	PRECOMPILE_DIR = PreComp
endif

# Do not compile po directory if WXRC is broken (gentoo wxWidgets 2.6.3.2)
SUBDIRS = $(PRECOMPILE_DIR) wxXtra wxOil Kernel tools @PODIR@

# We clean these here not in the wxOil makefile to prevent a loop
MOSTLYCLEANFILES=wxOil/xrc/xrc.d wxOil/xrc/xrc.stamp Resources/*

bin_PROGRAMS		= XaraLX
if DARWIN_LINK
    XaraLX_SOURCES	= Kernel/*.o wxOil/*.o tools/*.o wxXtra/*.o
    XaraLX_LDSOURCE = $(XaraLX_SOURCES)
else
    XaraLX_SOURCES	= Kernel/libKernel.a wxOil/libwxOil.a tools/libTools.a wxXtra/libwxXtra.a
    XaraLX_LDSOURCE = -Wl,--start-group $(XaraLX_SOURCES) -Wl,--end-group
endif
XaraLX_LDFLAGS		= --debug -L$(srcdir)/$(CDRAW_LIB_DIR) $(WX_LIBS) $(LIBS) $(LIBXML2_LIBS) -lCDraw
XaraLX_LD			= $(CXX)

if STATIC_ENABLE
	XaraLX_LDFLAGS	= -pthread \
		-L$(srcdir) -lCDraw \
		/usr/local/lib/libwx_gtk2u_xrc-2.6.a \
		/usr/local/lib/libwx_gtk2u_qa-2.6.a \
		/usr/local/lib/libwx_gtk2u_html-2.6.a \
		/usr/local/lib/libwx_gtk2u_adv-2.6.a \
		/usr/local/lib/libwx_gtk2u_core-2.6.a \
		/usr/local/lib/libwxregexu-2.6.a \
		/usr/local/lib/libwx_baseu_xml-2.6.a \
		/usr/local/lib/libwx_baseu_net-2.6.a \
		/usr/local/lib/libwx_baseu-2.6.a \
		/usr/X11R6/lib/libXrandr.a \
		/usr/X11R6/lib/libXext.a \
		/usr/X11R6/lib/libXinerama.a \
		/usr/X11R6/lib/libXxf86vm.a \
		-lgtk-x11-2.0 \
		-lgdk-x11-2.0 \
		-latk-1.0 \
		-lgdk_pixbuf-2.0 \
		-lpangoxft-1.0 \
		-lpangox-1.0 \
		-lpangoft2-1.0 \
		-lfontconfig \
		-lfreetype \
		-lpango-1.0 \
		-lgobject-2.0 \
		-lgmodule-2.0 \
		-lgthread-2.0 \
		-lglib-2.0 \
		/usr/lib/libpng.a \
		/usr/lib/libjpeg.a \
		/usr/lib/libtiff.a \
		/usr/lib/libexpat.a \
		/usr/lib/libz.a \
		/usr/lib/libxml2.a \
		/usr/lib/libm.a \
		-ldl \
		-static-libgcc
endif

XaraLX$(EXEEXT) : $(XaraLX_SOURCES)
	$(XaraLX_LD) $(XaraLX_LDSOURCE) $(XaraLX_LDFLAGS) -o $@

UNZIP = unzip

release: all

resources:
	@make -CwxOil xrc/resources.xrs ; \
	( rm -rf Resources ; mkdir -p Resources ; cd Resources ; $(UNZIP) -q ../wxOil/xrc/resources.xrs )

ACLOCAL_AMFLAGS = -I m4
