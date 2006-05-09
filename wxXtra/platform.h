// $Id: wxmousestate.h 751 2006-03-31 15:43:49Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#ifndef __WXXTRA_PLATFORM_H
#define __WXXTRA_PLATFORM_H

#include <wx/wx.h>

// This is an interface class which allows use of platform dependent functions
// without the user having to include any platform dependent includes.

class wxPlatformDependent : public wxObject
{
public:
    static bool Init(wxClassInfo * pClassInfo = NULL);

    static void DeInit();

private:
    static wxPlatformDependent * s_PlatformDependent;

public:
    static wxPlatformDependent * Get() {return s_PlatformDependent;}

protected:
    virtual void Initialise() {} // To override
    virtual void Deinitialise() {} // To override

public:
    virtual void InitWindow(wxWindow * pwxWindow) {} // To override

    virtual void FixUpdate(wxWindow * pwxWindow, bool flush=true);

#if defined( __WXGTK__ )
    virtual void ParseGtkRcString(char * rcstring);
    virtual void SetGtkWidgetName(wxWindow * pwxWindow, char * name);
#endif

	DECLARE_DYNAMIC_CLASS(wxPlatformDependent);
};

#endif // __WXXTRA_PLATFORM_H
