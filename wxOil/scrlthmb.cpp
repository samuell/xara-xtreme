// $Id$
/* @@tag:xara-cn@@ DO NOT MODIFY THIS LINE
================================XARAHEADERSTART===========================
 
               Xara LX, a vector drawing and manipulation program.
                    Copyright (C) 1993-2006 Xara Group Ltd.
       Copyright on certain contributions may be held in joint with their
              respective authors. See AUTHORS file for details.

LICENSE TO USE AND MODIFY SOFTWARE
----------------------------------

This file is part of Xara LX.

Xara LX is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License version 2 as published
by the Free Software Foundation.

Xara LX and its component source files are distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Xara LX (see the file GPL in the root directory of the
distribution); if not, write to the Free Software Foundation, Inc., 51
Franklin St, Fifth Floor, Boston, MA  02110-1301 USA


ADDITIONAL RIGHTS
-----------------

Conditional upon your continuing compliance with the GNU General Public
License described above, Xara Group Ltd grants to you certain additional
rights. 

The additional rights are to use, modify, and distribute the software
together with the wxWidgets library, the wxXtra library, and the "CDraw"
library and any other such library that any version of Xara LX relased
by Xara Group Ltd requires in order to compile and execute, including
the static linking of that library to XaraLX. In the case of the
"CDraw" library, you may satisfy obligation under the GNU General Public
License to provide source code by providing a binary copy of the library
concerned and a copy of the license accompanying it.

Nothing in this section restricts any of the rights you have under
the GNU General Public License.


SCOPE OF LICENSE
----------------

This license applies to this program (XaraLX) and its constituent source
files only, and does not necessarily apply to other Xara products which may
in part share the same code base, and are subject to their own licensing
terms.

This license does not apply to files in the wxXtra directory, which
are built into a separate library, and are subject to the wxWindows
license contained within that directory in the file "WXXTRA-LICENSE".

This license does not apply to the binary libraries (if any) within
the "libs" directory, which are subject to a separate license contained
within that directory in the file "LIBS-LICENSE".


ARRANGEMENTS FOR CONTRIBUTION OF MODIFICATIONS
----------------------------------------------

Subject to the terms of the GNU Public License (see above), you are
free to do whatever you like with your modifications. However, you may
(at your option) wish contribute them to Xara's source tree. You can
find details of how to do this at:
  http://www.xaraxtreme.org/developers/

Prior to contributing your modifications, you will need to complete our
contributor agreement. This can be found at:
  http://www.xaraxtreme.org/developers/contribute/

Please note that Xara will not accept modifications which modify any of
the text between the start and end of this header (marked
XARAHEADERSTART and XARAHEADEREND).


MARKS
-----

Xara, Xara LX, Xara X, Xara X/Xtreme, Xara Xtreme, the Xtreme and Xara
designs are registered or unregistered trademarks, design-marks, and/or
service marks of Xara Group Ltd. All rights in these marks are reserved.


      Xara Group Ltd, Gaddesden Place, Hemel Hempstead, HP2 6EX, UK.
                        http://www.xara.com/

=================================XARAHEADEREND============================
 */
/*
	The proportional scroller thumb window.


*/


#include "camtypes.h"

#include "scroller.h"
#include "scrlthmb.h"
#include "csrstack.h"
//#include "colordlg.h"	// For cursor resources, 'cos I don't like changing camelot.h!

#include "brushmsg.h"  // for the screenchange message

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

// Version information.
DECLARE_SOURCE("$Revision: 662 $");



/********************************************************************************************
>	void PatB(CDC* hDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> (Some MFC dude, actually)
	Created:	14/3/94
	Inputs:		hDC - destination DC
				x,y,dx,dy - rectangle to fill
				rgb - colour to fill with
	Purpose:	Paints a rectangle in the given (dithered) colour
				It looks pretty hideous, but this is how the MFC buttonbar does it...
				The conclusions that this leads to are left as an exercise for the reader.
				(OK, so they're not. It suggest that either MFC sux, or plotting text is
				easier/faster than creating a brush, in which case Windoze sux)
********************************************************************************************/

static void NEAR PASCAL PatB(CDC* cDC, INT32 x, INT32 y, INT32 dx, INT32 dy, COLORREF rgb)
{
	RECT rect;
	rect.left	= x;
	rect.top	= y;
	rect.right	= x + dx;
	rect.bottom	= y + dy;

	cDC->SetBkColor(rgb);
	cDC->ExtTextOut(0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}

static Cursor *AdjustDragCursor = NULL;
static INT32 DragCursorID = 0;

/*********************************************************************************************
>	CScrollerThumb::CScrollerThumb()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a CScrollerThumb C++ object.  Sets an internal variable to
				indicate that the thumb is currently NOT in dragging mode.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CScrollerThumb::Create(); CScroller::ConstructThumb()

**********************************************************************************************/ 

CScrollerThumb::CScrollerThumb() : Dragging(FALSE)
{
	// Nothing to do.
}




/********************************************************************************************

>	CScrollerThumb::~CScrollerThumb()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	17/11/93
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Destructor for class CScrollerThumb.  Doesn't precisely nothing.
	Errors:		-
	SeeAlso:	-

********************************************************************************************/

CScrollerThumb::~CScrollerThumb()
{
	// Nothing to do.
	// if (Dragging) ::ReleaseCapture();
}





/*********************************************************************************************
>	virtual BOOL CScrollerThumb::Create(LPCTSTR, LPCTSTR, DWORD style, const RECT& rect,
										CWnd* parent, UINT32 id, CCreateContext* ctxt = 0)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Two dummy long pointers, not used; the window-instance style flags; the size
				and position of the window; the parent; the numeric ID, the creation context.
	Outputs:	-
	Returns:	TRUE if the CScrollerThumb is successfully created (attached to a Windows
				interface element).
	Purpose:	Sets the class cursor used for the thumb window.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CScrollerThumb::CScrollerThumb(); CScroller::OnCreate()
**********************************************************************************************/ 

BOOL CScrollerThumb::Create(LPCTSTR, LPCTSTR, DWORD style, const RECT& rect,
							CWnd* parent, UINT32 id, CCreateContext* ctxt)
{
	return CWnd::Create(AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_BYTEALIGNCLIENT,
											Cursor::Arrow->Handle(),
											0,
											0),
						"",
						style | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS,
						rect,
						parent,
						id,
						ctxt);
}



/////////////////////////////////////////////////////////////////////////////
// CScrollerThumb implementation.
//

/*********************************************************************************************
>	void CScrollerThumb::PostMsgToScroller(UINT32 msg, CPoint pt) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		The message ID to posted; the (mouse) coordinates, relative to the thumb's
				client area, to be posted along with the message.
	Outputs:	-
	Returns:	-
	Purpose:	Posts a private message to the parent CScroller window.  Converts the
				coordinates passed in so that they are relative to the CScroller's client
				area.
	Errors:		-
	Scope:	    Private
	SeeAlso:    CScrollerThumb::OnLButtonDown(); CScrollerThumb::OnMouseMove();
				CScrollerThumb::OnLButtonUp()
**********************************************************************************************/ 

void CScrollerThumb::PostMsgToScroller(UINT32 msg, CPoint pt) const
{
	ClientToScreen(&pt);
	GetParent()->ScreenToClient(&pt);
	GetParent()->PostMessage(msg, pt.x, pt.y);
}




/////////////////////////////////////////////////////////////////////////////
// CScrollerThumb message handlers


/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnWin95Paint()

	Author:		Chris_Parks (Xara Group Ltd) <camelotdev@xara.com>
	Created:	4/10/95
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a WM_PAINT message sent by Windows.  Paints the light and dark
				shading around the edge of the thumb, so it looks like the genuine Windows
				95 article.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    -
**********************************************************************************************/ 
void CScrollerThumb::OnWin95Paint()
{
	// Create a device context to paint on, and find out the extent of this window.
	CPaintDC dc(this);
	dc.SetROP2(R2_COPYPEN);
	CRect rect;
	GetClientRect(&rect);
	
	// Fill the inside of the thumb, drawing a thin black border around it.
	CBrush interior(::GetSysColor(COLOR_BTNFACE));
	CBrush* oldbrush = dc.SelectObject(&interior);
		
	// Draw the shade inside the bottom and right edges.
	CPen shade(PS_SOLID, 1,RGB(255,0,0));// ::GetSysColor(COLOR_BTNSHADOW));
	CPen* oldpen = dc.SelectObject(&shade);

	dc.Rectangle(&rect);
	
	COLORREF TopLeft		= GetSysColor(COLOR_BTNFACE);
	COLORREF BotRight		= GetSysColor(COLOR_WINDOWFRAME);
	COLORREF TopLeftInside	= GetSysColor(COLOR_BTNHIGHLIGHT);
	COLORREF BotRightInside	= GetSysColor(COLOR_BTNSHADOW);

	INT32 x  = rect.left;
	INT32 y  = rect.top;
	INT32 dx = rect.Width();
	INT32 dy = rect.Height();

	// The plinth is always made up of 4 right-angle elements
	// When indented, the colours are swapped, but the elements retain position
	PatB(&dc, x, y, 1, dy-1, TopLeft);
	PatB(&dc, x, y, dx-1, 1, TopLeft);

	PatB(&dc, x+dx-1, y, 1, dy, BotRight);
	PatB(&dc, x, y+dy-1, dx, 1, BotRight);
  
	PatB(&dc, x+1, y+1, 1, dy-2, TopLeftInside);
	PatB(&dc, x+1, y+1, dx-2, 1, TopLeftInside);

	PatB(&dc, x+dx-2, y+1, 1, dy-2, BotRightInside);
	PatB(&dc, x+1, y+dy-2, dx-2, 1, BotRightInside);
  
	dc.SelectObject(oldbrush);
	dc.SelectObject(oldpen);
}

/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnPaint()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Responds to a WM_PAINT message sent by Windows.  Paints the light and dark
				shading around the edge of the thumb, so it looks like the genuine Windows
				article.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    -
**********************************************************************************************/ 

void CScrollerThumb::OnPaint()
{
	if(CScroller::IsChicago())
	{
		OnWin95Paint();
		return;
	}
	// Create a device context to paint on, and find out the extent of this window.
	CPaintDC dc(this);
	dc.SetROP2(R2_COPYPEN);
	CRect rect;
	GetClientRect(&rect);
	
	// Fill the inside of the thumb, drawing a thin black border around it.
	CBrush interior(::GetSysColor(COLOR_BTNFACE));
	CBrush* oldbrush = dc.SelectObject(&interior);
	dc.Rectangle(&rect);
	
	// Draw the shade inside the bottom and right edges.
	CPen shade(PS_SOLID, 2, ::GetSysColor(COLOR_BTNSHADOW));
	CPen* oldpen = dc.SelectObject(&shade);
	
	dc.MoveTo(rect.left + 2, rect.bottom - 2);
	dc.LineTo(rect.right - 2, rect.bottom - 2);
	dc.LineTo(rect.right - 2, rect.top + 2);

	// Draw the highlight, half as wide, inside the left and top edges.
	CPen highlight(PS_SOLID, 1, ::GetSysColor(COLOR_BTNHIGHLIGHT));
	dc.SelectObject(&highlight);
	dc.MoveTo(rect.left + 1, rect.bottom - 3);
	dc.LineTo(rect.left + 1, rect.top + 1);
	dc.LineTo(rect.right - 2, rect.top + 1);
	
	// For some reason Windows sometimes refuses to draw the first pixel of the highlight,
	// so draw that one ourselves.  I'm not wasting any more time debugging the display
	// driver!!
	dc.SetPixel(rect.left + 1, rect.bottom - 3, ::GetSysColor(COLOR_BTNHIGHLIGHT));
	
	// Deselect our drawing tools before they are destroyed.
	dc.SelectObject(oldbrush);
	dc.SelectObject(oldpen);
}



/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnLButtonDown(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Captures the mouse and sends a WM_GRABTHUMB message to the parent CScroller.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerThumb::PostMsgToScroller(); CScroller::OnGrabThumb();
				CScrollerThumb::OnMouseMove(); CScrollerThumb::OnLButtonUp()
**********************************************************************************************/ 

void CScrollerThumb::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	CWnd::OnLButtonDown(nFlags, point);
	if (!Dragging)
	{
		Dragging = TRUE;
		DragIsAdjust = FALSE;
		SetCapture();
		PostMsgToScroller(WM_GRABTHUMB, point);
	}
}



/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnRButtonDown(UINT32 nFlags, CPoint point)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com>
	Created:	16/11/94
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Captures the mouse and sends a WM_GRABTHUMBADJ message to the parent CScroller
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerThumb::PostMsgToScroller(); CScroller::OnGrabThumb();
				CScrollerThumb::OnMouseMove(); CScrollerThumb::OnLButtonUp()
**********************************************************************************************/ 

void CScrollerThumb::OnRButtonDown(UINT32 nFlags, CPoint point)
{
	CWnd::OnRButtonDown(nFlags, point);
	if (!Dragging)
	{
		Dragging = TRUE;
		DragIsAdjust = TRUE;
		SetCapture();

		if (((CScroller *)GetParent())->LinkedScroller != NULL)	// If adjust-drag is enabled...
		{
			// Try to set new pointer shape for the drag
			if (AdjustDragCursor == NULL)
				AdjustDragCursor = new Cursor(_R(IDC_ADJUSTSCROLL));
			if (AdjustDragCursor != NULL)
				DragCursorID = CursorStack::GPush(AdjustDragCursor);
		}

		PostMsgToScroller(WM_GRABTHUMBADJ, point);
	}
}



/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnMouseMove(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Sends a WM_DRAGTHUMB message to the parent CScroller.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerThumb::PostMsgToScroller(); CScroller::OnDragThumb();
				CScrollerThumb::OnLButtonDown(); CScrollerThumb::OnLButtonUp()
**********************************************************************************************/ 

void CScrollerThumb::OnMouseMove(UINT32 nFlags, CPoint point)
{
	CWnd::OnMouseMove(nFlags, point);
	if (Dragging) PostMsgToScroller((DragIsAdjust) ? WM_DRAGTHUMBADJ : WM_DRAGTHUMB, point);
}



/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnLButtonUp(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Releases the captured mouse and sends a WM_RELEASETHUMB message to the parent
				CScroller.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerThumb::PostMsgToScroller(); CScroller::OnDragThumb();
				CScrollerThumb::OnLButtonDown(); CScrollerThumb::OnMouseMove()
**********************************************************************************************/ 

void CScrollerThumb::OnLButtonUp(UINT32 nFlags, CPoint point)
{
	CWnd::OnLButtonUp(nFlags, point);
	if (Dragging)
	{
		Dragging = FALSE;
		::ReleaseCapture();
		PostMsgToScroller((DragIsAdjust) ? WM_RELEASETHUMBADJ : WM_RELEASETHUMB, point);
	}
	// tell people the screen has changed
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}



/*********************************************************************************************
>	afx_msg void CScrollerThumb::OnRButtonUp(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com> (Jason copied OnLButtonUp, 16/11/94)
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Releases the captured mouse and sends a WM_RELEASETHUMB message to the parent
				CScroller.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerThumb::PostMsgToScroller(); CScroller::OnDragThumb();
				CScrollerThumb::OnLButtonDown(); CScrollerThumb::OnMouseMove()
**********************************************************************************************/ 

void CScrollerThumb::OnRButtonUp(UINT32 nFlags, CPoint point)
{
	CWnd::OnRButtonUp(nFlags, point);
	if (Dragging)
	{
		Dragging = FALSE;
		::ReleaseCapture();

		// Restore old pointer shape after the drag
		if (AdjustDragCursor != NULL)
		{
			CursorStack::GPop(DragCursorID);
			delete AdjustDragCursor;
			AdjustDragCursor = NULL;
			DragCursorID = 0;
		}

		PostMsgToScroller((DragIsAdjust) ? WM_RELEASETHUMBADJ : WM_RELEASETHUMB, point);
	}
	// tell people things have changed on screen
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}



/////////////////////////////////////////////////////////////////////////////
// CScrollThumb message map.

BEGIN_MESSAGE_MAP(CScrollerThumb, CWnd)
	//{{AFX_MSG_MAP(CScrollerThumb)
		ON_WM_PAINT()
		ON_WM_LBUTTONDOWN()
		ON_WM_RBUTTONDOWN()
		ON_WM_MOUSEMOVE()
		ON_WM_LBUTTONUP()
		ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
