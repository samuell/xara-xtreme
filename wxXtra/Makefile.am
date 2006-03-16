#########################################################################
# LIBRARY SECTION
#########################################################################
# this is librairy that is part of main executable.  it's name is used for all
# of the other Makefile.am variables
noinst_LIBRARIES	= libwxXtra.a

# the application source, library search path, and link libraries
libwxXtra_a_SOURCES = manager.cpp wxmousestate.cpp doublebuffer.cpp

# make sure this does NOT have our include files in the path
libwxXtra_a_CXXFLAGS = @GTK_CFLAGS@

#libwxXtra_a_LDFLAGS = @GTK_LIBS@


