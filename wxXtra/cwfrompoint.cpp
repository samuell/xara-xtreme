// $Id$
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "cwfrompoint.h"

// wxChildWindowFromPoint
//
// This is a routine derived from wxFindWindowAtPoint (the unexported variant thereof).
// It finds the child window of a window passed as a parameter, which contains the point
// passed in. It fill find the deepest such child window. If no child window contains the
// point passed in, it will return a pointer to the window itself, unless that also
// does not contain the point in question, in which case it will return NULL.
// The "hidden" flag determines whether or not it should examine windows which are
// hidden (i.e. for which IsShown() is false). The routine can be passed a depth
// to recurse to, negative depth being infinite recursion, zero meaning do not look at
// child windows, and positive meaning a limited depth.
//
// Inputs:
//   wxWindow * win       The window to examine, plus children thereof
//   const wxPoint& pt    The coordinate, in screen coords
//   bool hidden          Flag, true to examine hidden windows as well (defaults to true)
//   int depth            Depth of recursion; zero to not recurse, positive for that number
//                        of levels, negative for infinite recursion (defaults to 1)
// Returns:
//   wxWindow *           Pointer to the deepest window searched containing pt, or NULL

wxWindow* wxChildWindowFromPoint(wxWindow* win, const wxPoint& pt, bool hidden /* =true */, int depth /* =1 */)
{
    bool shown = win->IsShown();

    if (win->IsKindOf(CLASSINFO(wxMDIChildFrame)))
    {
        wxWindow * pParent=win;
        // Look for a wxMDIParentFrame
        while ((pParent = /*assignment*/ pParent->GetParent()) && !(pParent->IsKindOf(CLASSINFO(wxMDIParentFrame)))) {}
        if (pParent)
        {
            // If this isn't the active child, then treat it as if it was hidden.
            if (((wxMDIParentFrame *)pParent)->GetActiveChild() != win)
                shown=FALSE;
        }
    }

    if (!(hidden || shown))
        return NULL;

    // Hack for wxNotebook case: at least in wxGTK, all pages
    // claim to be shown, so we must only deal with the selected one.
#if wxUSE_NOTEBOOK
    // If depth is >0, we want to descend as we have more searching to do. If depth
    // is negative, that means descend to infinite depth. So don't descend if depth
    // is exactly zero
    if (depth && (win->IsKindOf(CLASSINFO(wxNotebook))))
    {
      wxNotebook* nb = (wxNotebook*) win;
      int sel = nb->GetSelection();
      if (sel >= 0)
      {
        wxWindow* child = nb->GetPage(sel);
        wxWindow* foundWin = wxChildWindowFromPoint(child, pt, hidden, (depth<0)?depth:depth-1);
        if (foundWin)
           return foundWin;
      }
    }
#endif

    // If depth is >0, we want to descend as we have more searching to do. If depth
    // is negative, that means descend to infinite depth. So don't descend if depth
    // is exactly zero
    if (depth)
    {
        wxWindowList::compatibility_iterator node = win->GetChildren().GetLast();
        while (node)
        {
            wxWindow* child = node->GetData();
            wxWindow* foundWin = wxChildWindowFromPoint(child, pt, hidden, (depth<0)?depth:depth-1);
            if (foundWin)
              return foundWin;
            node = node->GetPrevious();
        }
    }

    wxPoint pos = win->GetPosition();
    wxSize sz = win->GetSize();
    if (win->GetParent() && !win->IsTopLevel()) // Do not translate coords in TLWs as they are already screen coords
    {
        pos = win->GetParent()->ClientToScreen(pos);
    }

    wxRect rect(pos, sz);
    if (rect.Inside(pt))
        return win;
    else
        return NULL;
}

// The same routine again but without the window hint

wxWindow* wxChildWindowFromPoint(const wxPoint& pt, bool hidden /* =true */, int depth /* =1 */)
{
    // Go backwards through the list since windows
    // on top are likely to have been appended most
    // recently.
    wxWindowList::compatibility_iterator node = wxTopLevelWindows.GetLast();
    while (node)
    {
        wxWindow* win = node->GetData();
        wxWindow* found = wxChildWindowFromPoint(win, pt, hidden, depth);
        if (found)
            return found;
        node = node->GetPrevious();
    }
    return NULL;
}


