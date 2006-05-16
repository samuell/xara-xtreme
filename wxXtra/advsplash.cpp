// $Id: doublebuffer.cpp 757 2006-04-01 16:04:45Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.
//
// Derived from wxWidgets splash.cpp, by Julian Smart

#include "advsplash.h"
#include <wx/splash.h>

#if defined(__WXGTK20__)
#include <gtk/gtk.h>
#endif

/*
 * wxAdvSplashScreen
 */

#define wxSPLASH_TIMER_ID 9999

IMPLEMENT_DYNAMIC_CLASS(wxAdvSplashScreen, wxFrame);

BEGIN_EVENT_TABLE(wxAdvSplashScreen, wxFrame)
    EVT_TIMER(wxSPLASH_TIMER_ID, wxAdvSplashScreen::OnNotify)
    EVT_CLOSE(wxAdvSplashScreen::OnCloseWindow)
END_EVENT_TABLE()

/* Note that unless we pass a non-default size to the frame, SetClientSize
 * won't work properly under Windows, and the splash screen frame is sized
 * slightly too small.
 */

wxAdvSplashScreen::wxAdvSplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
    wxFrame(parent, id, wxEmptyString, wxPoint(0,0), wxSize(100, 100), style)
{
    // At least for GTK+ 2.0, this hint is not available.
#if defined(__WXGTK20__)
#if GTK_CHECK_VERSION(2,2,0)
    gtk_window_set_type_hint(GTK_WINDOW(m_widget),
                             GDK_WINDOW_TYPE_HINT_SPLASHSCREEN);
#endif
#endif

    m_window = NULL;
    m_splashStyle = splashStyle;
    m_milliseconds = milliseconds;

    m_window = new wxAdvSplashScreenWindow(bitmap, this, wxID_ANY, pos, size, wxNO_BORDER);

    SetClientSize(bitmap.GetWidth()+2, bitmap.GetHeight()+2);

    if (m_splashStyle & wxSPLASH_CENTRE_ON_PARENT)
        CentreOnParent();
    else if (m_splashStyle & wxSPLASH_CENTRE_ON_SCREEN)
        CentreOnScreen();

    if (m_splashStyle & wxSPLASH_TIMEOUT)
    {
        m_timer.SetOwner(this, wxSPLASH_TIMER_ID);
        m_timer.Start(milliseconds, true);
    }

    Show(true);
    m_window->SetFocus();
#if defined( __WXMSW__ ) || defined(__WXMAC__)
    Update(); // Without this, you see a blank screen for an instant
#else
    wxYieldIfNeeded(); // Should eliminate this
#endif
}

wxAdvSplashScreen::~wxAdvSplashScreen()
{
    m_timer.Stop();
}

void wxAdvSplashScreen::OnNotify(wxTimerEvent& WXUNUSED(event))
{
    Close(true);
}

void wxAdvSplashScreen::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    m_timer.Stop();
    this->Destroy();
}

/*
 * wxAdvSplashScreenWindow
 */

BEGIN_EVENT_TABLE(wxAdvSplashScreenWindow, wxWindow)
#ifdef __WXGTK__
    EVT_PAINT(wxAdvSplashScreenWindow::OnPaint)
#endif
    EVT_ERASE_BACKGROUND(wxAdvSplashScreenWindow::OnEraseBackground)
    EVT_CHAR(wxAdvSplashScreenWindow::OnChar)
    EVT_MOUSE_EVENTS(wxAdvSplashScreenWindow::OnMouseEvent)
END_EVENT_TABLE()

wxAdvSplashScreenWindow::wxAdvSplashScreenWindow(const wxBitmap& bitmap, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style):
    wxWindow(parent, id, pos, size, style)
{
    m_bitmap = bitmap;

#if !defined(__WXGTK__) && wxUSE_PALETTE
    bool hiColour = (wxDisplayDepth() >= 16) ;

    if (bitmap.GetPalette() && !hiColour)
    {
        SetPalette(* bitmap.GetPalette());
    }
#endif

}

// VZ: why don't we do it under wxGTK?
#if !defined(__WXGTK__) && wxUSE_PALETTE
    #define USE_PALETTE_IN_SPLASH
#endif

static void wxDrawSplashBitmap(wxDC& dc, const wxBitmap& bitmap, int WXUNUSED(x), int WXUNUSED(y))
{
#if 0
    wxMemoryDC dcMem;

#ifdef USE_PALETTE_IN_SPLASH
    bool hiColour = (wxDisplayDepth() >= 16) ;

    if (bitmap.GetPalette() && !hiColour)
    {
        dcMem.SetPalette(* bitmap.GetPalette());
    }
#endif // USE_PALETTE_IN_SPLASH

    dcMem.SelectObject(bitmap);
    dc.Blit(0, 0, bitmap.GetWidth(), bitmap.GetHeight(), & dcMem, 0, 0);
    dcMem.SelectObject(wxNullBitmap);

#ifdef USE_PALETTE_IN_SPLASH
    if (bitmap.GetPalette() && !hiColour)
    {
        dcMem.SetPalette(wxNullPalette);
    }
#endif // USE_PALETTE_IN_SPLASH
#endif

	dc.DrawBitmap(bitmap, 0, 0);

}

void wxAdvSplashScreenWindow::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    wxPaintDC dc(this);
    if (m_bitmap.Ok())
        wxDrawSplashBitmap(dc, m_bitmap, 0, 0);
}

void wxAdvSplashScreenWindow::OnEraseBackground(wxEraseEvent& event)
{
    if (event.GetDC())
    {
        if (m_bitmap.Ok())
        {
            wxDrawSplashBitmap(* event.GetDC(), m_bitmap, 0, 0);
        }
    }
    else
    {
        wxClientDC dc(this);
        if (m_bitmap.Ok())
        {
            wxDrawSplashBitmap(dc, m_bitmap, 0, 0);
        }
    }
}

void wxAdvSplashScreenWindow::OnMouseEvent(wxMouseEvent& event)
{
    if (event.LeftDown() || event.RightDown())
        GetParent()->Close(true);
}

void wxAdvSplashScreenWindow::OnChar(wxKeyEvent& WXUNUSED(event))
{
    GetParent()->Close(true);
}

