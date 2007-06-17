///////////////////////////////////////////////////////////////////////////////
// Name:        wx/aui/floatpane.h
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id: floatpane.h,v 1.4 2006/07/04 15:26:30 AMB Exp $
// Copyright:   (C) Copyright 2005, Kirix Corporation, All Rights Reserved.
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

#ifndef _WXXTRA_FLOATPANE_H_
#define _WXXTRA_FLOATPANE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/wx.h>

#if wxUSE_AUI
#undef wxXTRA_AUI
#include <wx/aui/floatpane.h>
#else
#define wxXTRA_AUI 1

#include <wx/frame.h>

#if defined( __WXMSW__ ) || defined( __WXMAC__ )
#include "wx/minifram.h"
#define wxAuiFloatingFrameBaseClass wxMiniFrame
#else
#define wxAuiFloatingFrameBaseClass wxFrame
#endif

class WXDLLIMPEXP_AUI wxAuiFloatingFrame : public wxAuiFloatingFrameBaseClass
{
public:
    wxAuiFloatingFrame(wxWindow* parent,
                   wxAuiManager* owner_mgr,
                   const wxAuiPaneInfo& pane,
                   wxWindowID id = wxID_ANY
                   );
    ~wxAuiFloatingFrame();
    void SetPaneWindow(const wxAuiPaneInfo& pane);
private:
    void OnSize(wxSizeEvent& event);
    void OnClose(wxCloseEvent& event);
    void OnMoveEvent(wxMoveEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnMoveStart();
    void OnMoving(const wxRect& window_rect);
    void OnMoveFinished();
    void OnActivate(wxActivateEvent& event);
    static bool isMouseDown();
private:
    wxWindow* m_pane_window;    // pane window being managed
    bool m_moving;
    wxRect m_last_rect;
    wxSize m_last_size;

    wxAuiManager* m_owner_mgr;
    wxAuiManager m_mgr;

#ifndef SWIG
    DECLARE_EVENT_TABLE()
    DECLARE_CLASS(wxAuiFloatingFrameBaseClass)
#endif // SWIG
};

#endif // wxUSE_AUI
#endif //_WX_FLOATPANE_H_

