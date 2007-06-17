///////////////////////////////////////////////////////////////////////////////
// Name:        src/aui/floatpane.cpp
// Purpose:     wxaui: wx advanced user interface - docking window manager
// Author:      Benjamin I. Williams
// Modified by:
// Created:     2005-05-17
// RCS-ID:      $Id: floatpane.cpp,v 1.5 2006/07/05 16:38:01 BIW Exp $
// Copyright:   (C) Copyright 2005-2006, Kirix Corporation, All Rights Reserved
// Licence:     wxWindows Library Licence, Version 3.1
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "framemanager.h"
#include "floatpane.h"
#include "dockart.h"

#if wxXTRA_AUI

IMPLEMENT_CLASS( wxAuiFloatingFrame, wxAuiFloatingFrameBaseClass )

wxAuiFloatingFrame::wxAuiFloatingFrame(wxWindow* parent,
                wxAuiManager* owner_mgr,
                const wxAuiPaneInfo& pane,
                wxWindowID id /*= wxID_ANY*/)
                : wxAuiFloatingFrameBaseClass(parent, id, wxEmptyString,
                        pane.floating_pos, pane.floating_size,
                        wxRESIZE_BORDER | wxSYSTEM_MENU | wxCAPTION |
                        (pane.HasCloseButton()?wxCLOSE_BOX:0) |
                        wxFRAME_NO_TASKBAR |
                        wxFRAME_FLOAT_ON_PARENT | wxCLIP_CHILDREN |
                        (pane.IsFixed()?0:wxRESIZE_BORDER)
                        )
{
    m_owner_mgr = owner_mgr;
    m_moving = false;
    m_last_rect = wxRect();
    m_mgr.SetManagedWindow(this);
    SetExtraStyle(wxWS_EX_PROCESS_IDLE);
}

wxAuiFloatingFrame::~wxAuiFloatingFrame()
{
    m_mgr.UnInit();
}

void wxAuiFloatingFrame::SetPaneWindow(const wxAuiPaneInfo& pane)
{
    m_pane_window = pane.window;
    m_pane_window->Reparent(this);

    wxAuiPaneInfo contained_pane = pane;
    contained_pane.Dock().Center().Show().
                    CaptionVisible(false).
                    PaneBorder(false).
                    Layer(0).Row(0).Position(0);

    // Carry over the minimum size
    SetMinSize(pane.window->GetMinSize());

    m_mgr.AddPane(m_pane_window, contained_pane);
    m_mgr.Update();

    if (pane.min_size.IsFullySpecified())
    {
        // because SetSizeHints() calls Fit() too (which sets the window
        // size to its minimum allowed), we keep the size before calling
        // SetSizeHints() and reset it afterwards...
        wxSize tmp = GetSize();
        GetSizer()->SetSizeHints(this);
        SetSize(tmp);
    }

    SetTitle(pane.caption);

    if (pane.floating_size != wxDefaultSize)
    {
        SetSize(pane.floating_size);
    }
        else
    {
        wxSize size = pane.best_size;
        if (size == wxDefaultSize)
            size = pane.min_size;
        if (size == wxDefaultSize)
            size = m_pane_window->GetSize();
        if (pane.HasGripper())
        {
            if (pane.HasGripperTop())
                size.y += m_owner_mgr->m_art->GetMetric(wxAUI_ART_GRIPPER_SIZE);
            else
                size.x += m_owner_mgr->m_art->GetMetric(wxAUI_ART_GRIPPER_SIZE);
        }

        SetClientSize(size);
    }
}

void wxAuiFloatingFrame::OnSize(wxSizeEvent& event)
{
    m_owner_mgr->OnFloatingPaneResized(m_pane_window, event.GetSize());
}

void wxAuiFloatingFrame::OnClose(wxCloseEvent& evt)
{
    m_owner_mgr->OnFloatingPaneClosed(m_pane_window, evt);
    if (!evt.GetVeto())
        Destroy();
}

void wxAuiFloatingFrame::OnMoveEvent(wxMoveEvent& event)
{
#ifdef __WXGTK__
    // On wxGTK 2.6 and 2.7 for some unknown reason, wxSizeEvents are not
    // emitted for wxAuiFloatingFrames when they are manually resized.
    // See Bug #1528554.
    // However, it does (fortunately) wrongly emit wxMoveEvent in this scenario.
    // So we having on that to update the floating pane size - let's hope noone
    // fixes this useful bug, without fixing the above.
    m_owner_mgr->OnFloatingPaneResized(m_pane_window, GetSize());
#endif

    wxRect win_rect = GetRect();

    // skip the first move event
    if (m_last_rect.IsEmpty())
    {
        m_last_rect = win_rect;
        return;
    }

    // prevent frame redocking during resize
    if (m_last_rect.GetSize() != win_rect.GetSize())
    {
        m_last_rect = win_rect;
        return;
    }

    m_last_rect = win_rect;

    if (!isMouseDown())
        return;

    if (!m_moving)
    {
        OnMoveStart();
        m_moving = true;
    }

    OnMoving(event.GetRect());
}

void wxAuiFloatingFrame::OnIdle(wxIdleEvent& event)
{
    if (m_moving)
    {
        if (!isMouseDown())
        {
            m_moving = false;
            OnMoveFinished();
        }
            else
        {
            event.RequestMore();
        }
    }
}

void wxAuiFloatingFrame::OnMoveStart()
{
    // notify the owner manager that the pane has started to move
    m_owner_mgr->OnFloatingPaneMoveStart(m_pane_window);
}

void wxAuiFloatingFrame::OnMoving(const wxRect& WXUNUSED(window_rect))
{
    // notify the owner manager that the pane is moving
    m_owner_mgr->OnFloatingPaneMoving(m_pane_window);
}

void wxAuiFloatingFrame::OnMoveFinished()
{
    // notify the owner manager that the pane has finished moving
    m_owner_mgr->OnFloatingPaneMoved(m_pane_window);
}

void wxAuiFloatingFrame::OnActivate(wxActivateEvent& event)
{
    if (event.GetActive())
    {
        m_owner_mgr->OnFloatingPaneActivated(m_pane_window);
    }
}

// utility function which determines the state of the mouse button
// (independant of having a wxMouseEvent handy) - utimately a better
// mechanism for this should be found (possibly by adding the
// functionality to wxWidgets itself)
bool wxAuiFloatingFrame::isMouseDown()
{
    return wxGetMouseState().LeftDown();
}


BEGIN_EVENT_TABLE(wxAuiFloatingFrame, wxAuiFloatingFrameBaseClass)
    EVT_SIZE(wxAuiFloatingFrame::OnSize)
    EVT_MOVE(wxAuiFloatingFrame::OnMoveEvent)
    EVT_MOVING(wxAuiFloatingFrame::OnMoveEvent)
    EVT_CLOSE(wxAuiFloatingFrame::OnClose)
    EVT_IDLE(wxAuiFloatingFrame::OnIdle)
    EVT_ACTIVATE(wxAuiFloatingFrame::OnActivate)
END_EVENT_TABLE()

#endif // wxUSE_AUI
