#########################################################################
# LIBRARY SECTION
#########################################################################
# this is librairy that is part of main executable.  it's name is used for all
# of the other Makefile.am variables
noinst_LIBRARIES	= libwxXtra.a

# the application source, library search path, and link libraries
libwxXtra_a_SOURCES = \
	framemanager.cpp floatpane.cpp dockart.cpp \
	doublebuffer.cpp cwfrompoint.cpp combo.cpp combog.cpp odcombo.cpp xh_odcombo.cpp\
	platform.cpp advsplash.cpp treebook.cpp xh_treebk.cpp slidercombo.cpp xh_slidrcombo.cpp\
	gridcombo.cpp xh_gridcombo.cpp

# make sure this does NOT have our include files in the path
# Don't use GTK includes for now - you will need this with sub-2.6.3 but that's not supported
# right now
libwxXtra_a_CXXFLAGS = @GTK_CFLAGS@ $(XARALXCXXFLAGS)

# Don't load the GTK libraries into wxXtra any more - you will need this with sub-2.6.3 but
# that's not supported right now
#libwxXtra_a_LDFLAGS = @GTK_LIBS@


