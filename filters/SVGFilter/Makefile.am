bin_PROGRAMS = svgfilter svgfilterui

# Note TOPDIR seems to have one too many ".."s in to work. So don't use it
if LOCAL_XARLIB_ENABLE
# note includes are built, so no need to refer to topdir
XARLIB_CFLAGS2 = -DEXCLUDE_FROM_XARLIB -DUSE_NATIVE_XLONG -DBUILDSHADOWS -DOLD_MATRIX_TRANSFORMATIONS -DVECTOR_STROKING -DEXCLUDE_FROM_XARLIB -DNEW_SHADOW_RENDER -DNO_XARACMS \
				-DNEW_FEATURES -DSHOWPORTNOTE -DDO_EXPORT \
				-I../../xarlib/include $(WX_CXXFLAGS)
XARLIB_LIBS2 = -L../../xarlib -lXar
else
XARLIB_CFLAGS2 =
XARLIB_LIBS2 =
endif

# the application source, library search path, and link libraries
svgfilter_SOURCES = svgfilter.cpp svgfilter.h import.cpp svgimporter.cpp \
		    svgimporter.h export.cpp utils.cpp utils.h styles.cpp \
		    styles.h gradients.cpp gradients.h trans.cpp trans.h \
		    xargenerator.cpp xargenerator.h version.h
svgfilter_CPPFLAGS = $(XARLIB_CFLAGS2) $(LIBXML2_CFLAGS) $(XARLIB_CFLAGS)
svgfilter_LDADD = $(XARLIB_LIBS2) $(LIBXML2_LIBS) $(XARLIB_LIBS)

svgfilterui_SOURCES = svgfilterui.cpp svgfilterui.h svgexportdialog.cpp \
		      svgexportdialog.h version.h
svgfilterui_CPPFLAGS = $(XARLIB_CFLAGS) $(XARLIB_CFLAGS2) $(LIBXML2_CFLAGS)
svgfilterui_LDADD = $(XARLIB_LIBS) $(XARLIB_LIBS2) $(LIBXML2_LIBS)

EXTRA_DIST = svgfilterui.pjd svgfilter.xml svgfilterc.xml \
	     images/*.png samples/*.xar \
	     tests/*.svg tests/*.el tests/*.sh

install-data-local:
	mkdir -p $(DESTDIR)$(prefix)/share/xaralx/filters/
	$(INSTALL_DATA) $(srcdir)/svgfilter.xml $(DESTDIR)$(prefix)/share/xaralx/filters
	$(INSTALL_DATA) $(srcdir)/svgfilterc.xml $(DESTDIR)$(prefix)/share/xaralx/filters

uninstall-local:
	rm -f $(DESTDIR)$(prefix)/share/xaralx/filters/svgfilter.xml
	rm -f $(DESTDIR)$(prefix)/share/xaralx/filters/svgfilterc.xml
