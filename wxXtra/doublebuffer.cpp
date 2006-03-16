// $Id: wxmousestate.cpp 479 2006-02-15 18:30:13Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "doublebuffer.h"

#ifdef __WXGTK__

#include <wx/gtk/win_gtk.h> // For GTK_PIZZA etc.

void SetDoubleBuffer (wxWindow * pWindow, bool DoubleBuffer)
{
#ifdef __WXGTK20__
    gtk_widget_set_double_buffered( pWindow->m_wxwindow, DoubleBuffer );
#endif
    gtk_pizza_set_clear( GTK_PIZZA(pWindow->m_wxwindow), DoubleBuffer );
}

#else
void SetDoubleBuffer (wxWindow * pWindow, bool DoubleBuffer) {return;}
#endif

