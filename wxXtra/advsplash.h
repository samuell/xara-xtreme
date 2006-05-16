// $Id: doublebuffer.h 751 2006-03-31 15:43:49Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#ifndef __WXXTRA_ADVSPLASH_H
#define __WXXTRA_ADVSPLASH_H

#include <wx/wx.h>

class wxAdvSplashScreenWindow;

/*
 * wxAdvSplashScreen
 */

class wxAdvSplashScreen: public wxFrame
{
public:
    // for RTTI macros only
    wxAdvSplashScreen() {}
    wxAdvSplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds,
                   wxWindow* parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP);
    ~wxAdvSplashScreen();

    void OnCloseWindow(wxCloseEvent& event);
    void OnNotify(wxTimerEvent& event);

    long GetSplashStyle() const { return m_splashStyle; }
    wxAdvSplashScreenWindow* GetSplashWindow() const { return m_window; }
    int GetTimeout() const { return m_milliseconds; }

protected:
    wxAdvSplashScreenWindow*   m_window;
    long                    m_splashStyle;
    int                     m_milliseconds;
    wxTimer                 m_timer;

    DECLARE_DYNAMIC_CLASS(wxAdvSplashScreen)
    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxAdvSplashScreen)
};

/*
 * wxAdvSplashScreenWindow
 */

class wxAdvSplashScreenWindow: public wxWindow
{
public:
    wxAdvSplashScreenWindow(const wxBitmap& bitmap, wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);

    void OnPaint(wxPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnMouseEvent(wxMouseEvent& event);
    void OnChar(wxKeyEvent& event);

    void SetBitmap(const wxBitmap& bitmap) { m_bitmap = bitmap; }
    wxBitmap& GetBitmap() { return m_bitmap; }

protected:
    wxBitmap    m_bitmap;

    DECLARE_EVENT_TABLE()
    DECLARE_NO_COPY_CLASS(wxAdvSplashScreenWindow)
};



#endif // __WXXTRA_ADVSPLASH_H
