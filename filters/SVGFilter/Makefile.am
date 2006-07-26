bin_PROGRAMS = svgfilter svgfilterui

# the application source, library search path, and link libraries
svgfilter_SOURCES = svgfilter.cpp svgfilter.h import.cpp svgimporter.cpp \
		    svgimporter.h export.cpp utils.cpp utils.h styles.cpp \
		    styles.h version.h
svgfilter_CPPFLAGS = $(XARLIB_CFLAGS) $(LIBXML2_CFLAGS)
svgfilter_LDADD = $(XARLIB_LIBS) $(LIBXML2_LIBS)

svgfilterui_SOURCES = svgfilterui.cpp svgfilterui.h svgexportdialog.cpp \
		      svgexportdialog.h version.h
svgfilterui_CPPFLAGS = $(LIBXML2_CFLAGS)
svgfilterui_LDADD = $(LIBXML2_LIBS)

EXTRA_DIST = svgfilterui.pjd svgfilter.xml svgfilterc.xml \
	     images/*.png samples/*.xar tests/*.svg

install-data-local:
	mkdir -p $(prefix)/share/xaralx/filters/
	$(INSTALL_DATA) $(srcdir)/svgfilter.xml $(prefix)/share/xaralx/filters
	$(INSTALL_DATA) $(srcdir)/svgfilterc.xml $(prefix)/share/xaralx/filters

uninstall-local:
	rm -f $(prefix)/share/xaralx/filters/svgfilter.xml
	rm -f $(prefix)/share/xaralx/filters/svgfilterc.xml
