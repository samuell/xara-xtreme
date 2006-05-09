// $Id: wxmousestate.cpp 751 2006-03-31 15:43:49Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "platform.h"

wxPlatformDependent * wxPlatformDependent::s_PlatformDependent = NULL;
IMPLEMENT_DYNAMIC_CLASS(wxPlatformDependent, wxObject);

#if defined( __WXGTK__ )
#include <gtk/gtk.h>
#endif

// Platform specific Init code
bool wxPlatformDependent::Init(wxClassInfo * pClassInfo)
{
    if (!pClassInfo)
        pClassInfo = CLASSINFO(wxPlatformDependent);
	wxASSERT(pClassInfo != NULL);
	wxASSERT(pClassInfo->IsKindOf(CLASSINFO(wxPlatformDependent)));
    wxASSERT(s_PlatformDependent == NULL);
    s_PlatformDependent = (wxPlatformDependent *) pClassInfo->CreateObject();
    wxASSERT(s_PlatformDependent != NULL);
    if (!s_PlatformDependent) return false;

    Get()->Initialise();
	return true;
}

// Deinit static function
void wxPlatformDependent::DeInit()
{
    wxASSERT(s_PlatformDependent != NULL);
    Get()->Deinitialise();
    delete (s_PlatformDependent);
    s_PlatformDependent = NULL;
}

// Platform dependent stuff starts here

void wxPlatformDependent::FixUpdate(wxWindow * pwxWindow, bool flush /*=true*/)
{
#if defined( __WXGTK__ )
    // On GTK, in 2.6.3 and predecessor, wxWindow::Update does not update the window
    // where m_window is NULL, and thus misses native controls. We thus do it properly
    // here - we assume the patch to fix this will be taken in 2.6.4
    // This routine should be called after "update"
#if !wxCHECK_VERSION(2,6,4)
    // Update this one
    if (pwxWindow && pwxWindow->m_widget && pwxWindow->m_widget->window)
        gdk_window_process_updates( pwxWindow->m_widget->window, FALSE );
    // now recurse through children
    // Now process children if any
    wxWindowList::Node * pNode = pwxWindow->GetChildren().GetFirst();
    while (pNode)
    {
        FixUpdate(pNode->GetData(), false);
        pNode = pNode->GetNext();
    }
    if (flush)
        gdk_flush();
#endif
#endif
    return;
}

#if defined( __WXGTK__ )
void wxPlatformDependent::ParseGtkRcString(char * rcstring)
{
    gtk_rc_parse_string(rcstring);
	return;
}

void wxPlatformDependent::SetGtkWidgetName(wxWindow * pwxWindow, char * name)
{
    gtk_widget_set_name(pwxWindow->m_widget, name);
    return;
}

#endif

