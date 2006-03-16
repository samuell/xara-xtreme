// $Id: wxmousestate.h 479 2006-02-15 18:30:13Z alex $
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#ifndef __WXXTRA_WXMOUSESTATE_H
#define __WXXTRA_WXMOUSESTATE_H

#include <wx/wx.h>

// wxGetMouseState appeared in 2.6.3
#if wxCHECK_VERSION(2,6,3)
#undef HAVE_WXGETMOUSESTATE
#define HAVE_WXGETMOUSESTATE 1
#else
#undef HAVE_WXGETMOUSESTATE
#endif

#ifndef HAVE_WXGETMOUSESTATE

// wxMouseState is used to hold information about button and modifier state
// and is what is returned from wxGetMouseState.
class WXDLLEXPORT wxMouseState
{
public:
    wxMouseState()
        : m_x(0), m_y(0),
          m_leftDown(false), m_middleDown(false), m_rightDown(false),
          m_controlDown(false), m_shiftDown(false), m_altDown(false),
          m_metaDown(false)
    {}

    wxCoord     GetX() { return m_x; }
    wxCoord     GetY() { return m_y; }

    bool        LeftDown()    { return m_leftDown; }
    bool        MiddleDown()  { return m_middleDown; }
    bool        RightDown()   { return m_rightDown; }

    bool        ControlDown() { return m_controlDown; }
    bool        ShiftDown()   { return m_shiftDown; }
    bool        AltDown()     { return m_altDown; }
    bool        MetaDown()    { return m_metaDown; }
    bool        CmdDown()
    {
#if defined(__WXMAC__) || defined(__WXCOCOA__)
        return MetaDown();
#else
        return ControlDown();
#endif
    }

    void        SetX(wxCoord x) { m_x = x; }
    void        SetY(wxCoord y) { m_y = y; }

    void        SetLeftDown(bool down)   { m_leftDown = down; }
    void        SetMiddleDown(bool down) { m_middleDown = down; }
    void        SetRightDown(bool down)  { m_rightDown = down; }

    void        SetControlDown(bool down) { m_controlDown = down; }
    void        SetShiftDown(bool down)   { m_shiftDown = down; }
    void        SetAltDown(bool down)     { m_altDown = down; }
    void        SetMetaDown(bool down)    { m_metaDown = down; }

private:
    wxCoord     m_x;
    wxCoord     m_y;

    bool        m_leftDown;
    bool        m_middleDown;
    bool        m_rightDown;

    bool        m_controlDown;
    bool        m_shiftDown;
    bool        m_altDown;
    bool        m_metaDown;
};

WXDLLEXPORT wxMouseState wxGetMouseState();



#endif // HAVE_WXGETMOUSESTATE

#endif // __WXXTRA_WXMOUSESTATE_H
