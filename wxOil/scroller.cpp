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
//
// This defines the CScroller class, which implement RISCOS-style
// proportional scroll bars under Windows.
//


#include "camtypes.h"

#include "ccmaths.h"
#include "camdoc.h"
#include "ops.h"
//#include "scrcamvw.h"
#include "scroller.h"
//#include "scrlbutn.h"
//#include "scrlthmb.h"
#include "ensure.h"
#include "csrstack.h"
#include "document.h"
#include "camdoc.h"
#include "camframe.h"
#include "vstate.h"
#include "product.h"

//#include <limits.h>
#if 0
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif
#endif

DECLARE_SOURCE("$Revision$");

#define new CAM_DEBUG_NEW

/////////////////////////////////////////////////////////////////////////////
// CScrollerCorner window - fills in between a horizontal and vertical scroll
// bar.

BEGIN_EVENT_TABLE(CScrollerCorner, wxWindow)
	EVT_PAINT(CScrollerCorner::OnPaint)			
    EVT_LEFT_DOWN(CScrollerCorner::OnLButtonDown)
END_EVENT_TABLE()


/*********************************************************************************************
>	CScrollerCorner::CScrollerCorner()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	7th Sept 1993
	Purpose:	Constructs a scroller object.
	Scope:	    Protected

**********************************************************************************************/ 

CScrollerCorner::CScrollerCorner()
{
	//SizeBitmap.LoadOEMBitmap(OBM_SIZE);
}



/*********************************************************************************************
>	virtual BOOL CScrollerCorner::Create(LPCTSTR, LPCTSTR, DWORD style, const RECT& rect, 
							 			 CWnd* parent, UINT32 id)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Two dummy long pointers to constant TCHARs, the window-instance flags, a
				rectangle describing the position of the "corner", a pointer to the parent
				window, the numeric child window identifier, a pointer to a creation context.
	Outputs:	-
	Returns:	TRUE if the window is successfully created.
	Purpose:	Creates (attaches) the Windows interface element which corresponds to the
				C++ CScrollerCorner object.  The CScrollerCorner's background brush is
				set to the one used to colour scroll bars.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CCamView::OnCreate()

**********************************************************************************************/ 

BOOL CScrollerCorner::Create(LPCTSTR, LPCTSTR, DWORD style, const wxRect& rect, 
							 wxWindow* pParent, UINT32 id)
{
	return wxWindow::Create( pParent, id, rect.GetTopLeft(), rect.GetSize(), style );
/*
	return CWnd::Create(AfxRegisterWndClass(
							CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
							0,
							HBRUSH(COLOR_SCROLLBAR + 1),
							0),
						"",
						style | WS_CHILD | WS_CLIPSIBLINGS | WS_VISIBLE,
						rect,
						parent,
						id,
						ctxt);
*/
}



/////////////////////////////////////////////////////////////////////////////
// CScrollerCorner message response functions.

/*********************************************************************************************
>	afx_msg void CScrollerCorner::OnPaint(wxPaintEvent& event)

	Author:		Chris_Snook (Xara Group Ltd) <camelotdev@xara.com>
	Created:	8/10/95
	Purpose:	Paints the client area of the CScrollerCorner.  
	Scope:	    Protected
	SeeAlso:    CScrollerCorner::Create()

**********************************************************************************************/ 

void CScrollerCorner::OnPaint(wxPaintEvent& event)
{
#ifdef GERRYXXX
	CPaintDC dc(this);
	
	// Draw the resizing gadget.
	CPen nullpen(PS_NULL, 0, RGB(0, 0, 0));
	CPen* oldpen = dc.SelectObject(&nullpen);
	dc.SetROP2(R2_COPYPEN);

	// Create an appropriately coloured brush to fill the scroller's background.
	CBrush brush(::GetSysColor(COLOR_BTNFACE));
	CBrush* oldbrush = dc.SelectObject(&brush);

	// Draw the body of the scroller corner.
	CRect rClient;
	GetClientRect(&rClient);
	dc.Rectangle(&rClient);

	// Draw the size gadget on top if not maximised.
	BOOL fIsMaxed;
	GetMainFrame()->MDIGetActive(&fIsMaxed);
	if (!fIsMaxed)
	{
		CDC scrDC;
		scrDC.CreateCompatibleDC(&dc);
		CBitmap* pOldSrcBitmap = (CBitmap*) scrDC.SelectObject(&SizeBitmap);
		dc.BitBlt(0, 0, rClient.Width(), rClient.Height(), &scrDC, 0, 0, SRCCOPY);
		scrDC.SelectObject(pOldSrcBitmap);
	}

	// Deselect the drawing tools before destroying the DC.
	dc.SelectObject(oldbrush);
	dc.SelectObject(oldpen);
#endif
}



/*********************************************************************************************
>	afx_msg BOOL CScrollerCorner::OnSetCursor(CWnd* pWnd, UINT32 nHitTest, UINT32 nMessage)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	14 Oct 93
	Inputs:		"event" The standard SetCursor event, which contains all the info to handle
					the request
	Outputs:	-
	Returns:	TRUE, ie. the cursor has been set, and no further processing is necessary.
				FALSE if the cursor has not been set, e.g. this view is not active.
	Purpose:	Fakes the sizing gadget for our custom scrollbars.
	Errors:	    -
	Scope:	    Protected
	SeeAlso:	DocView::OnSetCursor

**********************************************************************************************/ 

void CScrollerCorner::OnSetCursor( wxSetCursorEvent& event )
{
#ifdef GERRYXXX
	// Child windows assume responsibility for setting their own cursor.
	if (pWnd != this) return FALSE;
	
	BOOL fIsMaxed;
	GetMainFrame()->MDIGetActive(&fIsMaxed);
	Cursor* pc = /*fIsMaxed ?*/ Cursor::Arrow; /*: Cursor::SizeNWSE;*/

	pc->SetActive();
	return TRUE;
#endif
}


/*********************************************************************************************
>	afx_msg void CScrollerCorner::OnLButtonDown(wxMouseEvent &event)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	5/10/95
	Purpose:	Send a fake non-client msg to our parent to allow sizing via this window.
**********************************************************************************************/ 

void CScrollerCorner::OnLButtonDown(wxMouseEvent &event)
{
#ifdef GERRYXXX
	CWnd::OnLButtonDown(nFlags, pt);

	CWnd* pParent = GetParent();
	CRect rWin;
	pParent->GetWindowRect(&rWin);

	// calc screen coords of bottom-right frame corner
	pt.x = rWin.Width() - 1;
	pt.y = rWin.Height() - 1;
	pParent->ClientToScreen(&pt);

	pParent->SendMessage(WM_NCLBUTTONDOWN, WPARAM(HTSIZE),
						 MAKELPARAM(WORD(pt.x), WORD(pt.y)));
#endif
}



/////////////////////////////////////////////////////////////////////////////
//
//
//	CWinScroller
//
//
/////////////////////////////////////////////////////////////////////////////

INT32 CWinScroller::ScrollerSize = 0;	// UIC 

/*********************************************************************************************
>	CWinScroller::CWinScroller()

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Purpose:	Default constructor for the CWinScroller class.
				Initialises various flags and variables with some reasonable-
				looking values.

**********************************************************************************************/ 

CWinScroller::CWinScroller(BOOL IsHorizontal)
{
	Horizontal = IsHorizontal;
#ifdef GERRYXXX
	Enabled = TRUE;
	Proportional = TRUE;
	// Set some default values for the range etc.  If we don't do that then
	// the scroll bars are invisible when first created!
	ThumbVal = MinVal = 0;
	Range = MaxVal = 100;
	LineSize = Delta = Granularity = 1;
	PageSize = 10;
	ScrollerSize = GetSystemMetrics(SM_CYHSCROLL);		// UIC
#endif
}



/*********************************************************************************************
>	virtual CWinScroller::~CWinScroller()

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Purpose:	Destroys a CWinScroller - deletes its child windows.

**********************************************************************************************/ 

CWinScroller::~CWinScroller()
{
}



/*********************************************************************************************
>	BOOL CWinScroller::Create(wxWindow* pParent, UINT32 nID, const wxPoint& pos, const wxSize& size)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		Same as CScrollBar::Create
	Returns:	TRUE if the scroller is successfully created.
	Purpose:	Calls CScrollBar::Create with appropriate horizontal or vertical style

**********************************************************************************************/ 

BOOL CWinScroller::Create(wxWindow* pParent, UINT32 nID, const wxPoint& pos, const wxSize& size)
{
	ENSURE(pParent != NULL, "CWinScroller objects must have a parent window!\n");
	INT32 style = IsHorizontal() ? wxSB_HORIZONTAL : wxSB_VERTICAL;
	return wxScrollBar::Create(pParent, nID, pos, size, style);
}

#ifdef GERRYXXX

/*********************************************************************************************
>	void CWinScroller::GetScrollPos(XLONG* pxl) const

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Outputs:	Changes the passed XLONG to the current position of the scroller thumb.
	Purpose:	Gets the thumb offset of the scroller.
	SeeAlso:    CScroller::SetScrollPos()

**********************************************************************************************/ 

void CWinScroller::GetScrollPos(XLONG* pxl) const
{
	*pxl = ThumbVal;
}

/*********************************************************************************************
>	void CWinScroller::SetProportional(BOOL bProp)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		A boolean which if TRUE means that the scroller should become "proportional"
	SeeAlso:    CScroller::SetProportional

**********************************************************************************************/ 

void CWinScroller::SetProportional(BOOL bProp)
{
	Proportional = bProp;
}

/*********************************************************************************************
>	void CWinScroller::MoveThumb(BOOL redraw)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		A boolean which if TRUE means that CCamView is told to redraw its client
				area to reflect the new position of the scroller thumbs.

	Purpose:	This function begins by posting a WM_SCROLLEREVENT message to the parent
				CCamView, indicating that the scroll offset has changed (possibly by some
				prior action by the CCamView) and that its client area needs repainting.

	Note:		In the custom scroller code on which this class is based, this really did
				redraw the thumb. With the OS scrollbars the only thing still needed from 
				it however is to inform our parent of the change.

	SeeAlso:    CScroller::MoveThumb(BOOL redraw)

**********************************************************************************************/ 

void CWinScroller::MoveThumb(BOOL redraw)
{
	// Inform owner that something is happening.  Note that SendMessage() *must* be used
	// here, not PostMessage().  The internal values of the scroller must have
	// been updated before this call, as CCamView will try to read the scroller's
	// thumb position whilst processing the sent message.
//	GetParent()->SendMessage(WM_SCROLLEREVENT, WPARAM(redraw), LPARAM(this));
}

/*********************************************************************************************
>	void CWinScroller::ClipThumbPos(const XLONG& n)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		An XLONG giving the proposed offset of the thumb within the scroller range.
	Purpose:	Makes sure that the new scroller thumb position is within the set range of
				the scroller and does not result in "overscroll" (when the area beyond the
				document is visible within the scrolled window).  Sets the scroller thumb
				position accordingly.

**********************************************************************************************/ 

void CWinScroller::ClipThumbPos(const XLONG& n)
{
	//	find out last possible position (from CScrollBar)
	XLONG lastpos = GetScrollLimit();

	if (n < MinVal)
		ThumbVal = MinVal;
	else if (n > lastpos)
		ThumbVal = lastpos;
	else
		ThumbVal = n;
	SnapToGrain(&ThumbVal);						// make sure the scroll position always	
}												// coincides with a grain boundary.

/*********************************************************************************************
>	void CWinScroller::SetGranularity(const XLONG& unit)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com> (from Justin CScroller code)
	Created:	6/10/04
	Inputs:		An XLONG giving the smallest possible change that can be made to a
				scroller value.  All scroll positions stored within the scroller are
				integer multiples of this value.
	Outputs:	-
	Returns:	-
	Purpose:	All scroll positions are adjusted to be multiples of this number.  This
				ensures, for example, that scroll offsets coincide with pixels.  It's
				easiest to think of this as the "units", in millipoints, that scroll
				offsets are measured in.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::ClipThumbPos(); CWinScroller::SnapToGrain()

**********************************************************************************************/ 

void CWinScroller::SetGranularity(const XLONG& unit)
{
	Granularity = unit;
}



/*********************************************************************************************
>	void CWinScroller::SnapToGrain(XLONG* pxl) const

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	13th Sept 1993
	Inputs:		A pointer to an XLONG.
	Outputs:	Adjusts the pointed to XLONG so that it is the largest multiple of the
				Granularity <= its original value.
	Returns:	-
	Purpose:	All scroll positions are adjusted to be multiples of this number.  This
				ensures, for example, that scroll offsets coincide with pixels.  It's
				easiest to think of this as the "units", in millipoints, that scroll
				offsets are measured in.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::SetGranularity(); CWinScroller::ClipThumbPos()

**********************************************************************************************/ 

void CWinScroller::SnapToGrain(XLONG* pxl) const
{
	*pxl = (*pxl / Granularity) * Granularity;
}



/*********************************************************************************************
>	void CWinScroller::SetScrollPos(const XLONG& n, BOOL redraw)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		An XLONG giving the offset of the thumb within the scroller range, and a
				boolean which if TRUE will update the scrollable client area to reflect
				the new thumb position.
	Outputs:	-
	Returns:	-
	Purpose:	Checks that the new offset is within the range (clipping it if it isn't)
				and moves the thumb.  If the scroller is currently disabled the new
				scroll position is noted but the thumb is not redrawn in the new
				position.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::GetScrollPos(); CWinScroller::MoveThumb(); CCamView::SetScrollOffsets()

**********************************************************************************************/ 

void CWinScroller::SetScrollPos(const XLONG& n, BOOL redraw)
{
	ClipThumbPos(n);
	MoveThumb(redraw && Enabled);

	// Set the position of the scroll box.
	CScrollBar::SetScrollPos((INT32)ThumbVal ,true);
}



/*********************************************************************************************
>	void CWinScroller::SetScrollRange(const XLONG& lo, const XLONG& hi, BOOL redraw)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		The "low" and "high" points of the scroller's range; a boolean which if TRUE
				indicates that the client area should be redrawn immediately to reflect
				the change in the range (and possible change in scroll offset).
	Outputs:	-
	Returns:	-
	Purpose:	Sets internal variables which hold the limits and the range of the scroller,
				according to the parameters.  Makes sure that the thumb remains "in range".
				If the scroller range is less than a page in size, the scroller is disabled.
				The size of the thumb is recalculated and the scroller is redrawn.

	Errors:		Raise an ERROR2 if the input is not within range for 32 bit signed integers

	Notes:		Because windows scrollbar ranges are defined as INT32, we need to check for
				potential loss of data in our xlong input. Although it is not currently possible 
				to create a camelot document large enought to break these INT32 boundaries, this 
				check will be useful to pinpoint bugs if camelot is ever extended to allow larger 
				documents (e.g. multiple spreads).

				Note that we could have just downcast the parameters to ints and relied on xlong 
				handling an overflow on performing the conversion, but according to the docs if 
				XLONG is not in the integer range conversion will produce some unexpected results.

	Scope:	    Public
	SeeAlso:    CWinScroller::EnableScroller(); CWinScroller::CalcThumbSize();
				CWinScroller::MoveThumb(); CWinScroller::GetScrollRange()

**********************************************************************************************/ 

void CWinScroller::SetScrollRange(const XLONG& lo, const XLONG& hi, BOOL redraw)
{
	const INT32 MIN= -2147483647 - 1; 
	const INT32 MAX=  2147483647;		
	if( lo < MIN || hi < MIN || lo > MAX || hi > MAX )
	{
		ERROR2RAW( "SetScrollRange limits exceeded!");
	}

	// Calculate the "direction" (Delta) and the limits of the scroller.
	MinVal = lo;
	SnapToGrain(&MinVal);
	MaxVal = hi;
	SnapToGrain(&MaxVal);
	Range = Abs(MaxVal - MinVal);
	Delta = (MaxVal > MinVal ? 1 : (MaxVal < MinVal ? -1 : 0));

	// If necessary adjust the position of the thumb, so that it remains within
	// the new range.
	ClipThumbPos(ThumbVal);
	MoveThumb(redraw);

	// Now pass minimum and maximum position values to the CScrollBar control. Also set the 
	// nPage value because otherwise CScrollBar will miscalculate the thumbsize when we are 
	// zooming back up from a view that is smaller than the render window (e.g 25% to 100%).

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE|SIF_RANGE;
	si.nPage = (INT32)PageSize;
	si.nMin = MinVal;     
	si.nMax = MaxVal; 
	SetScrollInfo(&si, true);
}



/*********************************************************************************************
>	void CWinScroller::SetLineSize(const XLONG& sz)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		An XLONG holding the size of a "line", which is the amount scrolled when a
				CWinScroller button control is clicked once.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the line size.  CCamView generally sets this to one tenth of the client
				area.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::GetLineSize(); CWinScroller::LineUp(); CWinScroller::LineDown();
				CCamView::OnSize()

**********************************************************************************************/ 

void CWinScroller::SetLineSize(const XLONG& sz)
{
	LineSize = sz;
	SnapToGrain(&LineSize);
}



/*********************************************************************************************
>	void CWinScroller::SetPageSize(const XLONG& sz)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Inputs:		An XLONG which holds the size of a "page" - the amount scrolled in response
				to a click on the scroller which misses the buttons and the thumb.
	Outputs:	-
	Returns:	-
	Purpose:	Sets the page size.  CCamView sets this to the size of the client area.
				Anything else will allow the scroller to scroll past the end of the document.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::GetPageSize(); CWinScroller::PageUp(); CWinScroller::PageDown();
				CCamView::OnSize()

**********************************************************************************************/ 

void CWinScroller::SetPageSize(const XLONG& sz)
{
	PageSize = sz;
	SnapToGrain(&PageSize);
//	CalcThumbSize();
	MoveThumb(FALSE);

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask = SIF_PAGE;
	si.nPage = (INT32)PageSize;
	SetScrollInfo(&si, true);
}



/*********************************************************************************************
>	void CWinScroller::LineUp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls "up" one line (ie. towards the low limit of the scroller range).
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::LineDown(); CWinScroller::SetLineSize(); CWinScroller::GetLineSize();
				CWinScroller::OnDecBtnClick(); CWinScroller::OnIncBtnClick()

**********************************************************************************************/ 

void CWinScroller::LineUp()
{
	SetScrollPos(ThumbVal - Delta * LineSize);
}



/*********************************************************************************************
>	void CWinScroller::LineDown()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		
	Outputs:	
	Returns:	
	Purpose:	Scrolls "down" one line (ie. towards the high limit of the scroller range).
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::LineUp(); CWinScroller::SetLineSize(); CWinScroller::GetLineSize();
				CWinScroller::OnDecBtnClick(); CWinScroller::OnIncBtnClick()

**********************************************************************************************/ 

void CWinScroller::LineDown()
{
	SetScrollPos(ThumbVal + Delta * LineSize);
}



/*********************************************************************************************
>	void CWinScroller::PageUp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls "up" one page (towards the low limit of the scroller range).
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::PageDown(); CWinScroller::SetPageSize(); CWinScroller::GetPageSize();
				CWinScroller::OnLButtonDown(); CWinScroller::OnRButtonDown();
				CWinScroller::HandleMouseDown(); CWinScroller::CalcPageUpDownRects()

**********************************************************************************************/ 

void CWinScroller::PageUp()
{
	SetScrollPos(ThumbVal - Delta * PageSize);
}



/*********************************************************************************************
>	void CWinScroller::PageDown()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Scrolls "down" one page (towards the low limit of the scroller range).
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::PageUp(); CWinScroller::SetPageSize(); CWinScroller::GetPageSize();
				CWinScroller::OnLButtonDown(); CWinScroller::OnRButtonDown();
				CWinScroller::HandleMouseDown(); CWinScroller::CalcPageUpDownRects()

**********************************************************************************************/ 

void CWinScroller::PageDown()
{
	SetScrollPos(ThumbVal + Delta * PageSize);
}



/*********************************************************************************************
>	void CWinScroller::ShowScroller(BOOL show)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A boolean which is FALSE if the scroller is to be hidden, TRUE if to be made
				visible.
	Outputs:	-
	Returns:	-
	Purpose:	Shows or hides this scroller.  Note that hiding a scroller and leaving an
				empty "hole" looks really ugly, so you probably won't be calling this very
				much.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CWinScroller::EnableScroller()

**********************************************************************************************/ 

void CWinScroller::ShowScroller(BOOL show)
{
	ShowWindow(show ? SW_SHOWNA : SW_HIDE);
}

/*********************************************************************************************

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>, modified by Marc
	Inputs:		A long pointer to a rectangle which describes the dimensions of the client
				area of the window containing the scroller.
	Outputs:	Changes *lpRect to the position a the scroll bar would occupy normally, ie. 
				along the bottom or side of the owning window.
	Returns:	-
	Purpose:	Called by CCamView when its size changes, so it knows where to reposition
				the CHorzScroller.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CCamView::OnSize()

**********************************************************************************************/ 
void CWinScroller::CalcPosFromParentClient(LPRECT lpRect)
{
	if(Horizontal)
	{
		CRect parentRect;
		GetParent()->GetClientRect(&parentRect);
		lpRect->left = parentRect.left+1;
		lpRect->top = parentRect.bottom - ::GetSystemMetrics(SM_CYHSCROLL) + 1;
		lpRect->right = parentRect.right - ::GetSystemMetrics(SM_CXVSCROLL) + 1;
		lpRect->bottom = parentRect.bottom + 1;
	}
	else
	{
		CRect parentRect;
		GetParent()->GetClientRect(&parentRect);
		lpRect->left = parentRect.right - ::GetSystemMetrics(SM_CXVSCROLL) + 1;
		lpRect->top = parentRect.top + 1;
		lpRect->right = parentRect.right + 1;
		lpRect->bottom = parentRect.bottom - ::GetSystemMetrics(SM_CYHSCROLL) + 1;
	}
}

/*********************************************************************************************
>	void CWinScroller::HandleScrollMessage(UINT32 nSBCode, UINT32 nPos)

	Author:		Marc_Power (Xara Group Ltd) <camelotdev@xara.com>
	Created:	6/10/04
	Purpose:	Process a windows scroller event

**********************************************************************************************/ 
void CWinScroller::HandleScrollMessage(UINT32 nSBCode, UINT32 nPos)
{
	switch(nSBCode)
	{
	case SB_PAGEDOWN:
		PageDown() ;
		break ;
	case SB_PAGEUP:
		PageUp() ;
		break ;
	case SB_LINEDOWN:
		LineDown() ;
		break ;
	case SB_LINEUP:
		LineUp();
		break ;
	case SB_THUMBTRACK:
	case SB_THUMBPOSITION:
		// See KB article Q166473 - nPos is a legacy 16 bit coordinate. 
		// Replace it with true 32-bit scroll position
		SCROLLINFO si;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_TRACKPOS;
		GetScrollInfo(&si);
		nPos = si.nTrackPos;

		XLONG pos = 0;
		pos +=nPos;
		SetScrollPos(pos , true);
		break;
	}
}

#endif
