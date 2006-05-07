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

