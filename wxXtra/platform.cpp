// $Id: wxmousestate.cpp 751 2006-03-31 15:43:49Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "platform.h"

#if defined( __WXGTK__ )

#include <gtk/gtk.h>

// Platform specific Init code
void InitPlatform()
{
    gtk_rc_parse_string("\n"
                        "       style \"gtk-scale-blip-style\"\n"
                        "       {\n"
                        "               GtkScale::slider-length=13\n"
                        "       }\n"
                        "\n"
                        "       widget_class \"*.GtkHScale\" style \"gtk-scale-blip-style\"\n"
                        "       widget_class \"*.GtkVScale\" style \"gtk-scale-blip-style\"\n"
                        "\n");

	return;
}

#else

void InitPlaform()
{
	return;
}

#endif