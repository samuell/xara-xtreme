// $Id$
/* @@tag:xara-cn-tp@@ THIRD PARTY COPYRIGHT */
// The following line makes normalize.pl skip type fixing
/* SKIPFIXTYPES: START */

// This file is derived from CVSHEAD wxWidgets and is thus
// under the wxWidgets / wxWindows license.

#include "cwfrompoint.h"

// This is an exact duplication of wxFindWindowAtPoint
// (the unexported variant thereof)

wxWindow* wxChildWindowFromPoint(wxWindow* win, const wxPoint& pt)
{
    if (!win->IsShown())
        return NULL;

    // Hack for wxNotebook case: at least in wxGTK, all pages
    // claim to be shown, so we must only deal with the selected one.
#if wxUSE_NOTEBOOK
    if (win->IsKindOf(CLASSINFO(wxNotebook)))
    {
      wxNotebook* nb = (wxNotebook*) win;
      int sel = nb->GetSelection();
      if (sel >= 0)
      {
        wxWindow* child = nb->GetPage(sel);
        wxWindow* foundWin = wxChildWindowFromPoint(child, pt);
        if (foundWin)
           return foundWin;
      }
    }
#endif

    wxWindowList::compatibility_iterator node = win->GetChildren().GetLast();
    while (node)
    {
        wxWindow* child = node->GetData();
        wxWindow* foundWin = wxChildWindowFromPoint(child, pt);
        if (foundWin)
          return foundWin;
        node = node->GetPrevious();
    }

    wxPoint pos = win->GetPosition();
    wxSize sz = win->GetSize();
    if (win->GetParent())
    {
        pos = win->GetParent()->ClientToScreen(pos);
    }

    wxRect rect(pos, sz);
    if (rect.Inside(pt))
        return win;
    else
        return NULL;
}


