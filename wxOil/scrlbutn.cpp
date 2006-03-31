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
// Custom Windows scroll bar button.  This object is used by the (private)
// implementation of CScroller.
//


#include "camtypes.h"

#include "scroller.h"
#include "scrlbutn.h"
#include "ensure.h"
#include "mainfrm.h"
#include "errors.h"
#include "brushmsg.h" 

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


DECLARE_SOURCE("$Revision$");




BEGIN_MESSAGE_MAP(CScrollerButton, CBitmapButton)
	//{{AFX_MSG_MAP(CScrollerButton)
		ON_WM_LBUTTONDOWN()
		ON_WM_LBUTTONUP()
		ON_WM_RBUTTONDOWN()
		ON_WM_RBUTTONUP()
		ON_WM_TIMER()
		ON_WM_CANCELMODE()
		ON_WM_LBUTTONDBLCLK()
		ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()




UINT32 CScrollerButton::BitmapNames[4][3] = 
{
	  { OBM_UPARROW,	OBM_UPARROWD,	OBM_UPARROWI },
	  { OBM_DNARROW,	OBM_DNARROWD,	OBM_DNARROWI },
	  { OBM_LFARROW,	OBM_LFARROWD,	OBM_LFARROWI },
	  { OBM_RGARROW,	OBM_RGARROWD,	OBM_RGARROWI }
};




/*********************************************************************************************
>	CScrollerButton::CScrollerButton(CScroller::Orientation ort, UINT32 msg)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		The orientation of the bitmap on the button's face, the message the button
				button should send the CScroller parent when it is clicked.
	Outputs:	-
	Returns:	-
	Purpose:	Constructs a CScrollerButton with the appropriate face.
	Errors:		ENSURE failure if, for any stupid reason, the orientation is invalid.
	Scope:	    Public
	SeeAlso:    CScrollerButton::Create(); CScroller::OnCreate()

**********************************************************************************************/ 
CScrollerButton::CScrollerButton(CScrollerButton::Orientation ort, UINT32 msg)
  : CBitmapButton(), Orient(ort), Message(msg)
{
	// So far, very little to do here.
	ENSURE(ort >= CScrollerButton::UP && ort <= CScrollerButton::RIGHT,
		   "Unknown orientation code passed to scroller button constructor!\n");
}



/*********************************************************************************************
>	virtual BOOL CScrollerButton::Create(LPCTSTR, DWORD style, const RECT& rect,
							 			 CWnd* parent, UINT32 id)
	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A dummy long pointer, not used; the window-instance style flags, a rectangle
				describing the button's size and position; a pointer to the parent window (a
				CScroller derivative); a numeric child window identifier.
	Outputs:	-
	Returns:	TRUE if the button is successfully created, and all bitmaps it requires
				are successfully extracted from the dislay driver.
	Purpose:	Try to connect the C++ button object to the appropriate Windows
				interface object.  As an aid to the programmer we load the bitmaps
				used for the button here, instead of requiring the owner to do it.
	Errors:		-
	Scope:	    Public
	SeeAlso:    CScrollerButton::CScrollerButton(); CScroller::OnCreate()

**********************************************************************************************/ 
BOOL CScrollerButton::Create(LPCTSTR, DWORD style, const RECT& rect,
							 CWnd* parent, UINT32 id)
{
	return CBitmapButton::Create("",
								 style | WS_CLIPSIBLINGS | WS_VISIBLE | WS_CHILD
								 | BS_OWNERDRAW | BS_PUSHBUTTON,
								 rect,
								 parent,
								 id)
		&& LoadOEMBitmaps(BitmapNames[Orient][0],			// button up
					      BitmapNames[Orient][1],		    // button down
					      BitmapNames[Orient][2]);   		// disabled
}



/*********************************************************************************************
>	virtual BOOL CScrollerButton::LoadOEMBitmaps(UINT32 upID, UINT32 downID, UINT32 disableID)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		The resource identifier of the bitmap displayed on the button when it is
				"up", ie. not clicked; the ID of the "down" bitmap; the ID of the "disabled"
				bitmap.
	Outputs:	-
	Returns:	TRUE if the bitmaps are successfully extracted from the display driver, FALSE
				if some essential bitmaps cannot be extracted.
	Purpose:	Some display drivers, such as the ET4000, do not contain a "disabled" bitmap
				for scroll bars	buttons.  In this case the function substitutes the "up"
				bitmap for the "disabled" bitmap.
	Errors:		-
	Scope:	    Private
	SeeAlso:    CScrollerButton::Create()

**********************************************************************************************/ 
BOOL CScrollerButton::LoadOEMBitmaps(UINT32 upID, UINT32 downID, UINT32 disableID)
{
	m_bitmap.DeleteObject();
	m_bitmapSel.DeleteObject();
	m_bitmapDisabled.DeleteObject();

#ifndef _MAC
	// The up/down bitmaps are essential and must be present.
	if (!m_bitmap.LoadOEMBitmap(upID) || !m_bitmapSel.LoadOEMBitmap(downID))
		return FALSE;
	
	// The disabled bitmap is not essential, and if not present, the up
	// bitmap can be substituted for it.
	if (!m_bitmapDisabled.LoadOEMBitmap(disableID))
		m_bitmapDisabled.LoadOEMBitmap(upID);
#endif
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// Implementation.


/*********************************************************************************************
>	void CScrollerButton::HandleButtonDown(BOOL reverse)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		A boolean which if TRUE indicates that the button is responding to a click
				with the right mouse button, so the scroll is in the opposite direction.
	Outputs:	-
	Returns:	-
	Purpose:	Captures the mouse, to guarantee receiving a button-up message some time in
				the future.  Sets a timer in case that the button is held down and further
				messages must be processed (Windows buttons only send one notification
				message, no matter how long they are held down for).  Sends a private
				"button clicked" message to the parent CScroller object.
	Errors:		-
	Scope:	    Private
	SeeAlso:    CScrollerButton::HandleButtonUp(); CScrollerButton::OnLButtonDown();
				CScrollerButton::OnRButtonDown(); CScrollerButton::OnTimer()
				CScroller::OnDecBtnClicked(); CScroller::OnIncBtnClicked()

**********************************************************************************************/ 
void CScrollerButton::HandleButtonDown(BOOL reverse)
{
	// On the initial click, we set up an auto-repeat timer to start after the configured
	// keyboard auto-repeat delay
	UINT32 CallbackTime = 1000;
	UINT32 RepsPerSec;

	::SystemParametersInfo(SPI_GETKEYBOARDDELAY, 0, &RepsPerSec, 0);
	RepsPerSec = 4 - RepsPerSec;
	if (RepsPerSec > 1)
		CallbackTime = (UINT32) 1000 / RepsPerSec;

	TimerOn = ::FixSetTimer(m_hWnd, 3, CallbackTime, 0);

	#ifdef	_DEBUG
		if (!TimerOn)
			TRACE( _T("Unable to allocate a timer in CScrollerButton::HandleButtonDown()!\n"));
	#endif	// _DEBUG

	SetCapture();
	GetParent()->SendMessage(Message, WPARAM(Reversed = reverse));
	SetState(TRUE);
}



/*********************************************************************************************
>	void CScrollerButton::HandleButtonUp()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Releases the captured mouse and kills the timer set by HandleButtonDown().
	Errors:		-
	Scope:	    Private
	SeeAlso:    CScrollerButton::HandleButtonDown(); CScrollerButton::OnLButtonUp();
				CScrollerButton::OnRButtonUp(); CScrollerButton::OnTimer()

**********************************************************************************************/ 
void CScrollerButton::HandleButtonUp()
{
	ReleaseCapture();
	if (TimerOn)
	{
		::FixKillTimer(m_hWnd, 3);
		TimerOn = FALSE;
	}
	SetState(FALSE);

	// tell people things have changed on screen
	BROADCAST_TO_ALL(ScreenChangeMsg(TRUE));
}



/////////////////////////////////////////////////////////////////////////////
// CScrollerButton message response functions.

/*********************************************************************************************
>	afx_msg void CScrollerButton::OnLButtonDown(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	We must override the default handling for push-buttons because scroll-bar
				buttons generate a sequence of messages until they are released, whilst
				normal buttons generate only one message when pressed.  This member function
				passes the message on to HandleButtonDown() with a FALSE parameter.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::HandleButtonDown(); CScrollerButton::OnLButtonUp()

**********************************************************************************************/ 
void CScrollerButton::OnLButtonDown(UINT32 nFlags, CPoint point)
{
	CBitmapButton::OnLButtonDown(nFlags, point);
	HandleButtonDown(FALSE);
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnLButtonUp(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes message on to HandleButtonUp()
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::HandleButtonUp(); CScrollerButton::OnLButtonDown()

**********************************************************************************************/ 
void CScrollerButton::OnLButtonUp(UINT32 nFlags, CPoint point)
{
	CBitmapButton::OnLButtonUp(nFlags, point);
	SetState(FALSE);
	HandleButtonUp();
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnRButtonDown(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	We must override the default handling for push-buttons because Windows
				buttons do not respond to clicks with the right mouse-button.  This member
				function passes the message on to HandleButtonDown() with a TRUE parameter,
				after calling CButton::SetState(TRUE) to "simulate" the button being clicked.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::HandleButtonDown(); CScrollerButton::OnRButtonUp()

**********************************************************************************************/ 
void CScrollerButton::OnRButtonDown(UINT32 nFlags, CPoint point)
{
	SetState(TRUE);
	HandleButtonDown(TRUE);
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnRButtonUp(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Passes message on to HandleButtonUp(), after calling CButton::SetState(FALSE)
				to draw the button "undepressed".
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::HandleButtonUp(); CScrollerButton::OnRButtonDown()

**********************************************************************************************/ 
void CScrollerButton::OnRButtonUp(UINT32 nFlags, CPoint point)
{
	SetState(FALSE);
	HandleButtonUp();
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnLButtonDblClk(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Calls CScrollerButton::OnLButtonDown(), to convert a double-click into
				a single click (MFC buttons don't normally respond to double-clicks).
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::OnLButtonDown(); CScrollerButton::OnRButtonDblClk()

**********************************************************************************************/ 
void CScrollerButton::OnLButtonDblClk(UINT32 nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnRButtonDblClk(UINT32 nFlags, CPoint point)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		Some flags detailing which buttons and keys were down when the mouse message
				was generated; the coordinates of the mouse cursor.
	Outputs:	-
	Returns:	-
	Purpose:	Calls CScrollerButton::OnRButtonDown(), to convert a double-click into
				a single click (MFC buttons don't normally respond to double-clicks).
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::OnRButtonDown(); CScrollerButton::OnLButtonDblClk()

**********************************************************************************************/ 
void CScrollerButton::OnRButtonDblClk(UINT32 nFlags, CPoint point)
{
	OnRButtonDown(nFlags, point);
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnTimer(UINT32 nIDEvent)

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		The timer's numeric identifier (not used).
	Outputs:	-
	Returns:	-
	Purpose:	Responds to the elapsed timer, set in HandleButtonDown(), by repeating the
				last "button-clicked" message sent to the parent CScroller.  This allows the
				button to send a stream of messages until it is released - normally, Windows
				buttons send only one message per click.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::HandleButtonDown(); CScrollerButton::HandleButtonUp()

**********************************************************************************************/ 
void CScrollerButton::OnTimer(UINT32)
{
	// This checks for auto-repeat on the scroll button at the keyboard auto-repeat rate
	UINT32 CallbackTime = 1000;
	UINT32 RepsPerSec;

	::SystemParametersInfo(SPI_GETKEYBOARDSPEED, 0, &RepsPerSec, 0);
	CallbackTime = (UINT32) 1000 / (RepsPerSec + 1);
	TimerOn = ::FixSetTimer(m_hWnd, 3, CallbackTime, 0);

#ifdef	_DEBUG
	if (!TimerOn) TRACE( _T("Unable to allocate a timer in CScrollerButton::OnTimer()!\n"));
#endif	// _DEBUG

	GetParent()->SendMessage(Message, WPARAM(Reversed));
}



/*********************************************************************************************
>	afx_msg void CScrollerButton::OnCancelMode()

	Author:		Justin_Flude (Xara Group Ltd) <camelotdev@xara.com>
	Created:	ages ago
	Inputs:		-
	Outputs:	-
	Returns:	-
	Purpose:	Sent by windows if the user hits ESCAPE or the dialog of another app. pops
				up in the middle of some mouse capturing.  Gives this window a chance to
				release the captured mouse and kill any running timer.
	Errors:		-
	Scope:	    Protected
	SeeAlso:    CScrollerButton::HandleButtonDown(); CScrollerButton::OnTimer();
				CScrollerButton::HandleButtonUp()

**********************************************************************************************/ 
void CScrollerButton::OnCancelMode()
{
	CBitmapButton::OnCancelMode();
	if (TimerOn) HandleButtonUp();
}



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
 

/********************************************************************************************
>	void PlotMaskedBitmap(CDC* destDC, CBitmap* srcBitmap, INT32 x, INT32 y, INT32 width, INT32 height)

	Author:		Jason_Williams (Xara Group Ltd) <camelotdev@xara.com> and Chris
	Created:	14/3/94
	Inputs:		destDC    - DC into which the BMP will be plotted
				srcBitmap - NULL, or the source bitmap
				x, y, width, height - rectangle to plot into
	Purpose:	Plots the given bitmap (srcDC) into the given rectangle of the destination
				(destDC).
				if srcDC == NULL, then just fills the button with grey, else...
********************************************************************************************/

// Raster Ops, used to...

// Plot colour in the glyph bitmaps transparent (grey) regions
#define ROP_DSPDxax  0x00E20746L

// Plot colour in the glyph bitmaps non-transparent (coloured) regions
#define ROP_PSDPxax  0x00B8074AL



BOOL CScrollerButton::PlotMaskedBitmap(CDC* destDC, CBitmap* srcBitmap, INT32 SrcOffset,
								INT32 xPlotOffset,INT32 yPlotOffset, INT32 width, INT32 height)
{
	
	ERROR2IF(srcBitmap == NULL,FALSE,"NULL Bitmap in PlotMaskedBitmap()");
	ERROR2IF(destDC == NULL,FALSE,"NULL DC in PlotMaskedBitmap()");

	// always fill the entire background with BTNFACE to make sure we don't
	// miss any pixels 

	PatB(destDC, 0, 0, width, height, GetSysColor(COLOR_BTNFACE));
	
	// create a screen dc
	CDC scrDC ;
	scrDC.CreateCompatibleDC(destDC);
	
	// and select the bitmap into it
	CBitmap* OldSrcBitmap = (CBitmap *) scrDC.SelectObject(srcBitmap);

	// create a mask for the button glyph... First a DC
	CDC hDCMono ;
	hDCMono.CreateCompatibleDC(NULL);

	// create a mono bitmap for the mask
	CBitmap hbmMono ; 
	hbmMono.CreateBitmap(width, height, 1, 1, NULL);
	
	CBitmap * oldmonobitmap = (CBitmap *)hDCMono.SelectObject(&hbmMono);

	// initalize the bitmapto white (all 1's)
	hDCMono.PatBlt(0, 0, width, height, WHITENESS);

	// plot all non-lightgrey  pixels in the glyph as black in our mask
	scrDC.SetBkColor( RGB(192,192,192));
	hDCMono.BitBlt(xPlotOffset,yPlotOffset, width, height, &scrDC,
			SrcOffset, 0, SRCCOPY);

	destDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
	destDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1

	// plot the glyph 
	destDC->BitBlt( xPlotOffset, yPlotOffset, width, height,
					&scrDC, SrcOffset, 0, SRCCOPY);
		
	// fill with the normal BTNFACE colour
	CBrush HighlightBrush ; 
	HighlightBrush.CreateSolidBrush(GetSysColor(COLOR_BTNFACE));

	CBrush* hbrOld = (CBrush *) destDC->SelectObject(&HighlightBrush);
	if (hbrOld != NULL)
	{
		// draw highlight color where we have 0's in the mask
		destDC->BitBlt( xPlotOffset,yPlotOffset, width, height,  &hDCMono, 0, 0, ROP_DSPDxax);
		destDC->SelectObject(hbrOld);
	}
	
	// clean up DC's
	hDCMono.SelectObject(oldmonobitmap);
	scrDC.SelectObject(OldSrcBitmap);
	return TRUE;
}


void CScrollerButton::DrawItem (LPDRAWITEMSTRUCT	lpDIS)
{


	ASSERT(lpDIS != NULL);
	// must have at least the first bitmap loaded before calling DrawItem
	ASSERT(m_bitmap.m_hObject != NULL);     // required

	// use the main bitmap for up, the selected bitmap for down
	CBitmap* pBitmap = &m_bitmap;
	UINT32 state = lpDIS->itemState;
	if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
		pBitmap = &m_bitmapSel;

#ifndef _MAC
	else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL)
#else
	else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL &&
			(GetParent()->GetStyle() & DS_WINDOWSUI))
#endif
		pBitmap = &m_bitmapFocus;   // third image for focused
	else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL)
		pBitmap = &m_bitmapDisabled;   // last image for disabled

	

	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
	
	// New masked version
	//------------------------------------------------------------------------------
	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	if(!IS_CHICAGO)  // UIC
	{
		// draw the whole button
		PlotMaskedBitmap(pDC,pBitmap,0,rect.left, rect.top, rect.Width(), rect.Height());
	}
	else
	{ 
		UINT32 Flags =0;
		switch(Orient)
		{
		case UP:
			Flags =	DFCS_SCROLLUP;
			break;
		case DOWN:
			Flags = DFCS_SCROLLDOWN;
			break;
		case LEFT:
			Flags = DFCS_SCROLLLEFT;
			break;
		case RIGHT:
			Flags = DFCS_SCROLLRIGHT;
			break;
		}
		if (state & ODS_SELECTED)
			Flags |= DFCS_PUSHED;

		pDC->DrawFrameControl(&rect,DFC_SCROLL, Flags);
	}


	//-------------------------------------------------------------------------------
	
	// Old "scrcopy" version
	//-------------------------------------------------------------------------------
	/*
	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap* pOld = memDC.SelectObject(pBitmap);
	if (pOld == NULL)
		return;     // destructors will clean up

	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);
	pDC->BitBlt(rect.left, rect.top, rect.Width(), rect.Height(),
		&memDC, 0, 0, SRCCOPY);

	memDC.SelectObject(pOld);
  	*/
	//-------------------------------------------------------------------------------
}
