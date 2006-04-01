// $Id$
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "doublebuffer.h"

#ifdef __WXGTK__

#if wxCHECK_VERSION(2,6,3)
void SetDoubleBuffer (wxWindow * pWindow, bool DoubleBuffer)
{
#ifdef __WXGTK20__
    pWindow->SetDoubleBuffered( DoubleBuffer );
#endif // __WXGTK20__
}

#else // version 2.6.3 

// this probably won't work without a longer link line - if you get a link
// error here that means you aren't using wxWidgets 2.6.3 release which
// is now the minimum version - this code should not be active
#include <wx/gtk/win_gtk.h> // For GTK_PIZZA etc.

void SetDoubleBuffer (wxWindow * pWindow, bool DoubleBuffer)
{
#ifdef __WXGTK20__
    gtk_widget_set_double_buffered( pWindow->m_wxwindow, DoubleBuffer );
#endif // __WXGTK20__
    gtk_pizza_set_clear( GTK_PIZZA(pWindow->m_wxwindow), DoubleBuffer );
}
#endif // version 2.6.3

#else // __WXGTK__
void SetDoubleBuffer (wxWindow * pWindow, bool DoubleBuffer) {return;}
#endif // __WXGTK__

